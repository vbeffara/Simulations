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
}
