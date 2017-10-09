#pragma once /// \file
#include <gsl/gsl>
#include <vb/Array.h>
#include <vb/Picture.h>

namespace vb {
	template <typename T> class Bitmap : public Picture, public Array<T> { public:
		Bitmap (int wd, int ht, T d = T());

		using Array<T>::at;   	using Array<T>::atp;
		using Array<T>::begin;	using Array<T>::end;
		using Array<T>::contains; // TODO: rename, clashes with Fl_Widget

		void	put 	(coo z, T const & c)	{ Array<T>::put(z,c);  step(); }
		void	putp	(coo z, T const & c)	{ Array<T>::putp(z,c); step(); }

		T	lazy	(coo z, std::function <T(coo)> f)	{ if (at(z) == dflt) put(z, f(z)); return at(z); }

		void fill (coo z, T c, int adj = 4);

	private:
		coo z0;	///< The coordinates of the origin (at(0) is there on screen).
		T dflt;	///< The default value.

	protected:
		void paint () override {
			int64_t ppp = pixel_w()/w();
			gsl::span <Color> stage ((Color *) cairo_image_surface_get_data (surface), ppp*w() + stride*(ppp*h()-1));

			for (int x=0; x<w(); ++x)
				for (int y=0; y<h(); ++y)
					for (int dx=0; dx<ppp; ++dx)
						for (int dy=0; dy<ppp; ++dy)
							stage[ppp*x+dx+stride*(ppp*y+dy)] = Color(at(coo(x,y)));
		}
	};

	template<typename T> Bitmap<T>::Bitmap (int wd, int ht, T d) :
		Picture(wd,ht), Array<T>(wd,ht,d), z0(0,0), dflt(d) {}

	template<typename T> void Bitmap<T>::fill (coo z, T c, int adj) {
		T in = at(z); if (in == c) return;
	    std::vector<coo> xy; xy.push_back(z); at(z) = c;

		while (xy.size()) {
			coo ij = xy.back(); xy.pop_back(); for (int d=0; d<adj; ++d) {
				coo nij = ij + dz[d]; if (contains(nij) && (at(nij) == in)) { xy.push_back(nij); at(nij)=c; }
			}
		}
	}
} // namespace vb
