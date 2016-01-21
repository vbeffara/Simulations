#pragma once /// \file
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <future>
#include <vb/Picture.h>
#include <vb/coo.h>

namespace vb {
	class Coloring : public Picture { public:
		Coloring (int wd, int ht, std::function <Color(coo)> ff);

		void tessel (coo ul, coo lr);

		Color * stage = nullptr;

		Color & at (coo z) { return stage[z.x + stride * z.y]; }

		std::function <Color(coo)> f;

		virtual void show() {
			Picture::show();
			stage = (Color *) (cairo_image_surface_get_data (surface));
			tessel ({0,0}, {pixel_w()-1,pixel_h()-1});
		}
	};

	Coloring::Coloring (int wd, int ht, std::function <Color(coo)> ff) :
		Picture(wd,ht), f(ff) {}

	template <typename T> std::function <Color(coo)> aa (std::function <Color(std::complex<Color>)> f) {
		return [&](coo c) {
			using cplx = std::complex<Color>;
			cplx z (c.x,c.y); int r(0), g(0), b(0), a(0);
			for (int i=-1; i<=1; ++i) for (int j=-1; j<=1; ++j) {
				Color c = f(z + cplx{i/3.0,j/3.0}); r+=c.r; g+=c.g; b+=c.b; a+=c.a;
			}
			return Color(r/9,g/9,b/9,a/9);
		};
	}

    void Coloring::tessel (coo ul, coo lr) {
		if (!visible()) return;

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
