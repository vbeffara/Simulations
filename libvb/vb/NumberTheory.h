#pragma once
#include <vb/Polynomial.h>

namespace vb {
	template <typename T> Polynomial<bigint> guess (T x, T eps);
	template <typename T> Polynomial<bigint> guess (T x, int leps, unsigned d);

	template <typename T> Polynomial<cpxint> guess (cpx_t<T>::type x, T eps);
	template <typename T> Polynomial<cpxint> guess (cpx_t<T>::type x, int leps, unsigned d);

	boost::optional<cln::cl_UP_R> guess (cln::cl_F x, int nd);
	boost::optional<cln::cl_UP_N> guess_c (cln::cl_N x, int nd);
	boost::optional<cln::cl_UP_N> guess_r (cln::cl_N x, int nd);
}
