#pragma once /// @file
#include <vb/config.h>
#include <complex>
#include <iostream>

#ifdef HAVE_QUADMATH
#include <boost/multiprecision/float128.hpp>
#include <quadmath.h>
#endif

#ifdef HAVE_GMP
#include <boost/multiprecision/gmp.hpp>
#endif

namespace vb {
	using cpx = std::complex<double>;
	using lcpx = std::complex<long double>;

	const cpx I(0,1);
    const cpx dzc[4] = { cpx(1,0), cpx(0,1), cpx(-1,0), cpx(0,-1) };

    template <typename T> void foi (std::ostream & os, T x, bool no1 = false, bool nom1 = false) {
		T eps = pow(T(10),-os.precision());
		if (fabs(x-lround(x)) <= eps) {
			if (nom1 && (lround(x)==-1)) { os << "-"; return; }
			if ((!no1) || (lround(x)!=1)) { T r(lround(x)); os << long(r); }
		} else os << x;
	}

    static std::ostream & operator<< (std::ostream & os, cpx z) {
		double eps = pow(10.0,-os.precision());
		if (fabs(imag(z)) <= eps) { foi(os,real(z)); return os; }
		if (fabs(real(z)) <= eps) { foi(os,imag(z),true,true); os << " I"; return os; }
		bool neg = false; if (imag(z)<0) { z = conj(z); neg = true; }
		os << "("; foi(os,real(z)); os << (neg ? " - " : " + "); foi(os,imag(z),true); os << " I)"; return os;
    }

    static std::ostream & operator<< (std::ostream & os, lcpx z) {
		double eps = pow(10.0,-os.precision());
		if (fabs(imag(z)) <= eps) { foi(os,real(z)); return os; }
		if (fabs(real(z)) <= eps) { foi(os,imag(z),true,true); os << " I"; return os; }
		bool neg = false; if (imag(z)<0) { z = conj(z); neg = true; }
		os << "("; foi(os,real(z)); os << (neg ? " - " : " + "); foi(os,imag(z),true); os << " I)"; return os;
    }

#ifdef HAVE_QUADMATH
	using boost::multiprecision::float128;
	using qcpx = std::complex<float128>;

    static std::ostream & operator<< (std::ostream & os, qcpx z) {
		double eps = pow(10.0,-os.precision());
		if (fabs(imag(z)) <= eps) { foi(os,real(z)); return os; }
		if (fabs(real(z)) <= eps) { foi(os,imag(z),true,true); os << " I"; return os; }
		bool neg = false; if (imag(z)<0) { z = conj(z); neg = true; }
		os << "("; foi(os,real(z)); os << (neg ? " - " : " + "); foi(os,imag(z),true); os << " I)"; return os;
    }
#endif

#ifdef HAVE_GMP
	using bigint = boost::multiprecision::number<boost::multiprecision::gmp_int>;
	using cpxint = std::complex<bigint>;
	using gmp100 = boost::multiprecision::mpf_float_100;
	// using gmp100 = boost::multiprecision::number<boost::multiprecision::gmp_float<100>,boost::multiprecision::et_off>;
	using cpx100 = std::complex<gmp100>;

    template <typename T> void foii (std::ostream & os, T x, bool no1 = false, bool nom1 = false) {
		if (nom1 && (x == T(-1))) { os << "-"; return; }
		if ((!no1) || (x != T(1))) { os << x; }
	}

    static std::ostream & operator<< (std::ostream & os, cpxint z) {
		if (imag(z) == 0) { foii(os,real(z)); return os; }
		if (real(z) == 0) { foii(os,imag(z),true,true); os << " I"; return os; }
		bool neg = false; if (imag(z)<0) { z = conj(z); neg = true; }
		os << "("; foii(os,real(z)); os << (neg ? " - " : " + "); foii(os,imag(z),true); os << " I)"; return os;
    }

    static std::ostream & operator<< (std::ostream & os, cpx100 z) {
		double eps = pow(10.0,-os.precision());
		if (fabs(imag(z)) <= eps) { foi(os,real(z)); return os; }
		if (fabs(real(z)) <= eps) { foi(os,imag(z),true,true); os << " I"; return os; }
		bool neg = false; if (imag(z)<0) { z = conj(z); neg = true; }
		os << "("; foi(os,real(z)); os << (neg ? " - " : " + "); foi(os,imag(z),true); os << " I)"; return os;
    }
#endif
};
