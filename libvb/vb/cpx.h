#pragma once /// @file
#include <complex>
#include <iostream>

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
		long double eps = pow(10.0,-os.precision());
		if (fabs(imag(z)) <= eps) return os << (real(z)>=0 ? " " : "") << real(z); else
		if (fabs(real(z)) <= eps) return os << imag(z) << " I"; else
		return os << "(" << real(z) << " + " << imag(z) << " I)";
    }
};
