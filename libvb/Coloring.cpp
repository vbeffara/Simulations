#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <future>
#include <vb/Coloring.h>

namespace vb {
	Coloring::Coloring (cpx z1_, cpx z2_, int n, std::function <Color(cpx)> f_) :
			Picture(n,n*imag(z2_-z1_)/real(z2_-z1_)), z1(z1_), z2(z2_), f(f_) {}

	void Coloring::show () {
		Picture::show();
		stage = (Color *) (cairo_image_surface_get_data (surface));
		eps = real(z2-z1)/pixel_w();
		tessel ({0,0}, {pixel_w()-1,pixel_h()-1});
	}

	void Coloring::scale (double s) { cpx mid = (z1+z2)/2.0; z1 = mid + s * (z1-mid); z2 = mid + s * (z2-mid); }
	void Coloring::shift (cpx z) { z1 += z; z2 += z; }

	cpx Coloring::c_to_z (coo c) const { return z1 + cpx(c) * eps; }

	Color & Coloring::at (coo z) const { return stage[z.x + stride * z.y]; }

	Color Coloring::color (coo c) const {
		if (!aa) return f(c_to_z(c));
		cpx z = c_to_z(c); int r(0), g(0), b(0), a(0);
		for (int i=-1; i<=1; ++i) for (int j=-1; j<=1; ++j) {
			Color c = f (z + eps*cpx(i,j)/3.0);
			r+=c.r; g+=c.g; b+=c.b; a+=c.a;
		}
		return Color(r/9,g/9,b/9,a/9);
	}

    void Coloring::line (coo s, coo d, int l) { for (int i=0; i<l; ++i) { coo c = s+d*i; at(c) = color(c); } }

    void Coloring::go (coo ul, coo lr) {
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
    }

    void Coloring::tessel (coo ul, coo lr) {
        auto main = std::async (std::launch::async, [&]() {
            line (ul, coo(1,0), lr.x-ul.x);
            line (coo(lr.x,ul.y), coo(0,1), lr.y-ul.y);
            line (lr, coo(-1,0), lr.x-ul.x);
            line (coo(ul.x,lr.y), coo(0,-1), lr.y-ul.y);
            go (ul, lr);
        } );

        while (main.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) update(); main.get();
    }

	int Coloring::handle (int event) {
		if (event == FL_KEYDOWN) switch (Fl::event_key()) {
			case '-':     	scale (1.25);                    	show(); return 1; break;
			case '+':     	scale (0.80);                    	show(); return 1; break;
			case FL_Left: 	shift (cpx(+.1,0) * real(z2-z1));	show(); return 1; break;
			case FL_Right:	shift (cpx(-.1,0) * real(z2-z1));	show(); return 1; break;
			case FL_Up:   	shift (cpx(0,-.1) * real(z2-z1));	show(); return 1; break;
			case FL_Down: 	shift (cpx(0,+.1) * real(z2-z1));	show(); return 1; break;
		}
		return Picture::handle(event);
	}
}
