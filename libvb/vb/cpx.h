#pragma once /// @file
#include <cmath>
#include <complex>
#include <iostream>
#include <boost/multiprecision/gmp.hpp>
#include <cln/cln.h>

namespace vb {
	using cpx = std::complex<double>;

	const cpx I(0,1);
    const cpx dzc[4] = { cpx(1,0), cpx(0,1), cpx(-1,0), cpx(0,-1) };

    template <typename T> void foi (std::ostream & os, T x, bool no1 = false, bool nom1 = false) {
		T eps = pow(T(10),-os.precision());
		if (fabs(x-lround(x)) <= eps) {
			if (nom1 && (lround(x)==-1)) { os << "-"; return; }
			if ((!no1) || (lround(x)!=1)) { T r(lround(x)); os << long(r); }
		} else os << x;
	}

	using bigint = cln::cl_I;
	using cpxint = cln::cl_N; // no complex integer type in CLN...
	using real_t = cln::cl_R;
	using complex_t = cln::cl_N;

	using gmpint = boost::multiprecision::number<boost::multiprecision::gmp_int,boost::multiprecision::et_off>;

	template <typename T> class cpx_t { public: using type = std::complex<T>; };
	template <> class cpx_t <real_t> { public: using type = complex_t; };
	template <> class cpx_t <bigint> { public: using type = cpxint; };

	static cln::cl_R norm (const cln::cl_N & z) { return realpart(z)*realpart(z) + imagpart(z)*imagpart(z); }

	static cln::cl_R pow (const cln::cl_R & z, int k) { return expt(z,k); }
	static cln::cl_N pow (const cln::cl_N & z, const cln::cl_N & k) { return expt(z,k); }
	using std::pow;

	static cln::cl_R real (const cln::cl_N & z) { return realpart(z); }
	using std::real;

	static cln::cl_R imag (const cln::cl_N & z) { return imagpart(z); }
	using std::imag;

	static bool isnormal (const cln::cl_N &) { return true; }
	using std::isnormal;

	static cln::cl_R log10 (const cln::cl_R & z) { return realpart(log(z)); }
	using std::log10;

	template <typename T> int to_int (const T & z) { return int(z); }
	template <> int to_int (const cln::cl_I & z) { return cl_I_to_int(z); }
	template <> int to_int (const cln::cl_R & z) { return cl_I_to_int(floor1(z)); }

    template <typename T> void foii (std::ostream & os, T x, bool no1 = false, bool nom1 = false) {
		if (nom1 && (x == T(-1))) { os << "-"; return; }
		if ((!no1) || (x != T(1))) { os << x; }
	}

  //   inline std::ostream & operator<< (std::ostream & os, cpxint z) {
		// if (imag(z) == 0) { foii(os,real(z)); return os; }
		// if (real(z) == 0) { foii(os,imag(z),true,true); os << " I"; return os; }
		// bool neg = false; if (imag(z)<0) { z = conj(z); neg = true; }
		// os << "("; foii(os,real(z)); os << (neg ? " - " : " + "); foii(os,imag(z),true); os << " I)"; return os;
  //   }

    template <typename T> std::ostream & operator<< (std::ostream & os, std::complex<T> z) {
		double eps = pow(10.0,-os.precision());
		if (fabs(imag(z)) <= eps) { foi(os,real(z)); return os; }
		if (fabs(real(z)) <= eps) { foi(os,imag(z),true,true); os << " I"; return os; }
		bool neg = false; if (imag(z)<0) { z = conj(z); neg = true; }
		os << "("; foi(os,real(z)); os << (neg ? " - " : " + "); foi(os,imag(z),true); os << " I)"; return os;
    }

    template <typename T> void disp (std::ostream & os, const std::string & label, const T & x) {
		os << label << x << std::endl;
	}
}
