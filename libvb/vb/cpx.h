#pragma once /// @file
#include <complex>
#include <iostream>

#ifdef HAVE_QUADMATH
#include <boost/multiprecision/float128.hpp>
#include <quadmath.h>
#endif

namespace vb {
	using cpx = std::complex<double>;
	using lcpx = std::complex<long double>;

	const cpx I(0,1);
    const cpx dzc[4] = { cpx(1,0), cpx(0,1), cpx(-1,0), cpx(0,-1) };

    static std::ostream & operator<< (std::ostream & os, const cpx & z) {
		double eps = pow(10.0,-os.precision());
		if (fabs(imag(z)) <= eps) return os << (real(z)>=0 ? " " : "") << real(z); else
		if (fabs(real(z)) <= eps) return os << imag(z) << " I"; else
		return os << "(" << real(z) << " + " << imag(z) << " I)";
    }

    static std::ostream & operator<< (std::ostream & os, const lcpx & z) {
		double eps = pow(10.0,-os.precision());
		if (fabs(imag(z)) <= eps) return os << (real(z)>=0 ? " " : "") << real(z); else
		if (fabs(real(z)) <= eps) return os << imag(z) << " I"; else
		return os << "(" << real(z) << " + " << imag(z) << " I)";
    }

#ifdef HAVE_QUADMATH
	using boost::multiprecision::float128;
	using qcpx = std::complex<float128>;

    static std::ostream & operator<< (std::ostream & os, const qcpx & z) {
		double eps = pow(10.0,-os.precision());
		if (fabs(imag(z)) <= eps) return os << (real(z)>=0 ? " " : "") << real(z); else
		if (fabs(real(z)) <= eps) return os << imag(z) << " I"; else
		return os << "(" << real(z) << " + " << imag(z) << " I)";
    }
#else
    using float128 = long double;
#endif
};
