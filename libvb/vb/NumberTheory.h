#pragma once
#include <vb/Polynomial.h>

namespace vb {
	template <typename T> Polynomial<bigint> guess (T x, T eps);
	template <typename T> Polynomial<bigint> guess (T x, int leps, unsigned d);

	template <typename T> Polynomial<cpxint> guess (std::complex<T> x, T eps);
	template <typename T> Polynomial<cpxint> guess (std::complex<T> x, int leps, unsigned d);
}
