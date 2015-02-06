#pragma once
#include <vb/Constellation.h>
#include <vb/Minimizer.h>
#include <vb/NumberTheory.h>
#include <vb/Spheroidal.h>
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
}
