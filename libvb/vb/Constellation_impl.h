#pragma once
#include <vb/Constellation.h>
#include <iomanip>

namespace vb {
	template <typename T> Constellation<T>::Constellation () {};

	template <typename T> template <typename U> Constellation<T>::Constellation (const Constellation<U> & C) {
		for (auto zd : C.b) b.push_back({std::complex<T>(zd.z), zd.d});
		for (auto zd : C.w) w.push_back({std::complex<T>(zd.z), zd.d});
		for (auto zd : C.f) f.push_back({std::complex<T>(zd.z), zd.d});
		for (auto z : C.p)  p.push_back(std::complex<T>(z));
		dim = b.size() + w.size() + f.size() + p.size();
	};

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

	template <typename T> void Constellation<T>::draw (Image & img, bool smooth, bool wow) const {
		auto bd = bounds(); cplx center { (bd.first+bd.second) / T(2) }; T scale = T(.7) * abs(bd.first-bd.second);

		int l = img.w(); img.start = img.now(); for (auto & c : img) c = Color(0);

		auto f = [&](cplx z) {
			z = conj(z)*T(2.0/l) + cplx{-1,1}; z = center + scale*z; z = (*this)(z);
			double s = .8; if (wow) {
				cplx w = cplx(4)*z-cplx(2); w *= cplx(1) + sqrt(cplx(1)-cplx(4)/(w*w)); w/=2;
				s = 1/double(norm(w)); s = .8*(1-sqrt(1-s))*(1-sqrt(1-s));
			}
			return HSV ((imag(z)>0)?0:.5, .8, s);
		};

		img.tessel(0,0,img.w()-1,img.h()-1, smooth ? aa<T>(f) : [&](coo c){ return f(cplx(c.x,c.y)); });
		img.update();
	};
}
