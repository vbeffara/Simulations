#pragma once
#include <vb/Constellation.h>
#include <iomanip>

namespace vb {
	template <typename T> T Constellation<T>::findn () {
		Vector<cplx> x = vec();
		T c = cost(), old_c = c + T(1); auto old_x = x;
		while (c<old_c) {
			std::cerr << c << "             \r"	;
			old_c = c; old_x = x;
			x -= solve(jacvcost(),vcost());
			readvec(x); c = cost();
		}
		readvec(old_x); return old_c;
	}

	template <typename T> T Constellation<T>::cost () const {
	    T out(0); for (auto z : vcost()) out += norm(z);
		T bound = sqrt(sqrt(out));
		for (int i=0; i<b.size(); ++i) for (int j=0; j<b.size(); ++j) if (i!=j) if (abs(reduce(b[i].z-b[j].z)) < bound) out += T(1);
		for (int i=0; i<w.size(); ++i) for (int j=0; j<w.size(); ++j) if (i!=j) if (abs(reduce(w[i].z-w[j].z)) < bound) out += T(1);
		for (int i=0; i<f.size(); ++i) for (int j=0; j<f.size(); ++j) if (i!=j) if (abs(reduce(f[i].z-f[j].z)) < bound) out += T(1);
	    if (std::isnan(double(out))) out = T(1.234567e89);
	    return out;
	}

	template <typename T> void Constellation<T>::draw (Image & img, bool smooth) const {
		auto bd = bounds(); cplx center { (bd.first+bd.second) / T(2) }; T scale = T(.7) * abs(bd.first-bd.second);

		int l = img.w(); img.start = img.now(); for (auto & c : img) c = Color(0);

		auto f = [&](cplx z) {
			z = conj(z)*T(2.0/l) + cplx{-1,1}; z = center + scale*z; z = (*this)(z);
			// z = cplx(4)*z-cplx(2); z *= cplx(1) + sqrt(cplx(1)-cplx(4)/(z*z));
			// T h = T(.5) + arg (z) / T(2*M_PI); T s = T(1) / sqrt(norm(z));
			// return HSV(double(h),double(s),double(s));
			return imag(z)>0 ? Color(200,250,250) : Color(200,200,250);
		};

		img.tessel(0,0,img.w()-1,img.h()-1, smooth ? aa<T>(f) : [&](coo c){ return f(cplx(c.x,c.y)); });
		img.update();
	};
}
