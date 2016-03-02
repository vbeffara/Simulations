#pragma once
#include <vb/Polynomial.h>
#include <cln/cln.h>

namespace vb {
	template <typename T> Polynomial<bigint> guess (T x, T eps);
	template <typename T> Polynomial<bigint> guess (T x, int leps, unsigned d);

	template <typename T> Polynomial<cpxint> guess (std::complex<T> x, T eps);
	template <typename T> Polynomial<cpxint> guess (std::complex<T> x, int leps, unsigned d);

	boost::optional<cln::cl_UP_R> guess (cln::cl_F x, int nd);
}
