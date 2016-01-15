#pragma once /// \file
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <future>
#include <vb/Picture.h>
#include <vb/Array.h>

namespace vb {
	template <typename T> class Bitmap : public Picture, public Array<T> { public:
		Bitmap (int wd, int ht, T d=0);

		using Array<T>::at;   	using Array<T>::atp;
		using Array<T>::begin;	using Array<T>::end;
		using Array<T>::contains; // TODO: rename, clashes with Fl_Widget

		void	put 	(coo z, T const & c)	{ Array<T>::put(z,c);  step(); }
		void	putp	(coo z, T const & c)	{ Array<T>::putp(z,c); step(); }

		T	lazy	(coo z, std::function <T(coo)> f)	{ if (at(z) == dflt) put(z, f(z)); return at(z); }

		void fill (coo z, T c, int adj = 4);
		void tessel (coo ul, coo lr, std::function <T(coo)> f);

	private:
		Color * stage;	///< The raw pixel data of the screen representation.
		coo z0;       	///< The coordinates of the origin (at(0) is there on screen).
		T dflt;       	///< The default value.

	protected:
		virtual void paint () {
			for (int x=0; x<w(); ++x) for (int y=0; y<h(); ++y) stage[x+stride*y] = at(coo(x,y));
		}
	};

	template<typename T> Bitmap<T>::Bitmap (int wd, int ht, T d) :
		Picture(wd,ht), Array<T>(wd,ht,d), stage ((Color *) (cairo_image_surface_get_data (surface))), z0(0), dflt(d) {}

	template<typename T> void Bitmap<T>::fill (coo z, T c, int adj) {
		T in = at(z); if (in == c) return;
	    std::vector<coo> xy; xy.push_back(z); at(z) = c;

		while (xy.size()) {
			coo ij = xy.back(); xy.pop_back(); for (int d=0; d<adj; ++d) {
				coo nij = ij + dz[d]; if (contains(nij) && (at(nij) == in)) { xy.push_back(nij); at(nij)=c; }
			}
		}
	}

	template <typename T> std::function <Color(coo)> aa (std::function <Color(std::complex<T>)> f) {
		return [&](coo c) {
			using cplx = std::complex<T>;
			cplx z (c.x,c.y); int r(0), g(0), b(0), a(0);
			for (int i=-1; i<=1; ++i) for (int j=-1; j<=1; ++j) {
				Color c = f(z + cplx{i/3.0,j/3.0}); r+=c.r; g+=c.g; b+=c.b; a+=c.a;
			}
			return Color(r/9,g/9,b/9,a/9);
		};
	}

    template <typename T> void Bitmap<T>::tessel (coo ul, coo lr, std::function <T(coo)> f) {
        std::function<void(coo,coo,int)> line = [&] (coo s, coo d, int l) {
            cilk_for (int i=0; i<l; ++i) { at(s+d*i) = f(s+d*i); }
        };

        std::function<void(coo,coo)> go = [&] (coo ul, coo lr) {
            Color tmp = at(ul); bool mono = true; coo z = ul;
            for (; mono && (z != coo {lr.x,ul.y}); z += {1,0}) mono = mono && (at(z) == tmp);
            for (; mono && (z != coo {lr.x,lr.y}); z += {0,1}) mono = mono && (at(z) == tmp);
            for (; mono && (z != coo {ul.x,lr.y}); z += {-1,0}) mono = mono && (at(z) == tmp);
            for (; mono && (z != coo {ul.x,ul.y}); z += {0,-1}) mono = mono && (at(z) == tmp);

            int size = std::min(lr.x-ul.x,lr.y-ul.y); if (size <= 1) return;
            if (mono) { for (int i=ul.x+1; i<lr.x; ++i) for (int j=ul.y+1; j<lr.y; ++j) at(coo(i,j)) = tmp; return; }

            coo ul_ = (lr.x-ul.x > lr.y-ul.y) ? coo {(ul.x+lr.x)/2,ul.y} : coo {ul.x,(ul.y+lr.y)/2};
            coo lr_ = (lr.x-ul.x > lr.y-ul.y) ? coo {(ul.x+lr.x)/2,lr.y} : coo {lr.x,(ul.y+lr.y)/2};
            coo dd_ = (lr.x-ul.x > lr.y-ul.y) ? coo {0,1} : coo {1,0};
            line (ul_,dd_,size);

            cilk_spawn go (ul,lr_); go (ul_,lr); cilk_sync;
        };

        auto main = std::async (std::launch::async, [&]() {
            line (ul, coo(1,0), lr.x-ul.x);
            line (coo(lr.x,ul.y), coo(0,1), lr.y-ul.y);
            line (lr, coo(-1,0), lr.x-ul.x);
            line (coo(ul.x,lr.y), coo(0,-1), lr.y-ul.y);
            go (ul, lr);
        } );

        while (main.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) update(); main.get();
    }

}
