#pragma once /// @file
#include <boost/multiprecision/float128.hpp>
#include <complex>
#include <iostream>
#include <quadmath.h>

namespace vb {
	using boost::multiprecision::float128;

	using cpx = std::complex<double>;
	using lcpx = std::complex<long double>;
	using qcpx = std::complex<float128>;

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

    static std::ostream & operator<< (std::ostream & os, const qcpx & z) {
		double eps = pow(10.0,-os.precision());
		if (fabs(imag(z)) <= eps) return os << (real(z)>=0 ? " " : "") << real(z); else
		if (fabs(real(z)) <= eps) return os << imag(z) << " I"; else
		return os << "(" << real(z) << " + " << imag(z) << " I)";
    }
};
