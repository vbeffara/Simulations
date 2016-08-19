#pragma once
#include <vb/Polynomial.h>
#include <boost/optional.hpp>

namespace vb {
	boost::optional<cln::cl_UP_R> guess (cln::cl_R x, int nd);
	boost::optional<cln::cl_UP_N> guess_c (cln::cl_N x, int nd);
	boost::optional<cln::cl_UP_N> guess_r (cln::cl_N x, int nd);
}
