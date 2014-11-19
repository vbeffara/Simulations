#pragma once
#include <vb/cpx.h>

// zeta = sigma'/sigma
// wp = - zeta'
// (wp')^2 = 4 wp^3 - g_2 wp - g_3
// wp'' = 6 wp^2 - g_2/2
// wp''' = 12 wp wp'

// z = \pi u / (2\omega_1)
// \zeta(u) = u \eta_1 / \omega_1 + ( \pi / (2\omega_1) ) \partial_z \theta_1(z,q)
// wp(u) = (\pi/2\omega_1)^2 ( \theta_1'''(0,q) / 3\theta'(0,q) - \partial_z^2 \theta_1(z,q) )

// e_1 =

namespace vb {
	static unsigned long long fact   	(unsigned long long n)                      	{ return (n<2 ? 1 : n*fact(n-1));  	}
	static unsigned long long binom  	(unsigned long long n, unsigned long long k)	{ return fact(n)/fact(k)/fact(n-k);	}
	static unsigned long long catalan	(unsigned long long n)                      	{ return binom(2*n,n)/(n+1);       	}

	template <typename T> std::complex<T> q_ (const std::complex<T> & tau) { return exp(std::complex<T>(0,1)*T(4*atan(T(1)))*tau); }

	template <typename T> std::complex<T> theta1_ (const std::complex<T> & z, const std::complex<T> & q) {
		std::complex<T> out(0), old(1);
		for (int n=0; out!=old; ++n) { old=out; out += 2.0 * pow(-1,n) * pow(q, (n+.5)*(n+.5)) * sin(cpx(2*n+1)*z); }
		return out;
	}

	template <typename T> std::complex<T> theta1prime_ (const std::complex<T> & z, const std::complex<T> & q) {
		std::complex<T> out(0), old(1);
		for (int n=0; out!=old; ++n) { old=out; out += 2.0 * pow(-1,n) * pow(q, (n+.5)*(n+.5)) * cpx(2*n+1) * cos(cpx(2*n+1)*z); }
		return out;
	}

	static cpx theta1second_ (cpx z, cpx q) {
		cpx out(0), old(1);
		for (int n=0; out!=old; ++n) { old=out; out -= 2.0 * pow(-1,n) * pow(q, (n+.5)*(n+.5)) * pow(cpx(2*n+1),2) * sin(cpx(2*n+1)*z); }
		return out;
	}

	static cpx theta1third_ (cpx z, cpx q) {
		cpx out(0), old(1);
		for (int n=0; out!=old; ++n) { old=out; out -= 2.0 * pow(-1,n) * pow(q, (n+.5)*(n+.5)) * pow(cpx(2*n+1),3) * cos(cpx(2*n+1)*z); }
		return out;
	}

	static cpx theta2_ (cpx z, cpx q) {
		cpx out(0), old(1);
		for (int n=0; out!=old; ++n) { old=out; out += 2.0 * pow(q, (n+.5)*(n+.5)) * cos(cpx(2*n+1)*z); }
		return out;
	}

	static cpx theta3_ (cpx z, cpx q) {
		cpx out(1), old(2);
		for (int n=1; out!=old; ++n) { old=out; out += 2.0 * pow(q, n*n) * cos(cpx(2*n)*z); }
		return out;
	}

	static cpx theta4_ (cpx z, cpx q) {
		cpx out(1), old(2);
		for (int n=1; out!=old; ++n) { old=out; out += 2.0 * pow(-q, n*n) * cos(cpx(2*n)*z); }
		return out;
	}

	template <typename T> std::complex<T> eta1_ (const std::complex<T> & q) {
		return - ( 16*atan(1)*atan(1)/ 6 ) * theta1third_(0,q) / theta1prime_(std::complex<T>(0),q);
	}

	template <typename T> std::complex<T> sigma_ (const std::complex<T> & z, const std::complex<T> & q) {
		return exp(eta1_(q)*z*z) * theta1_(M_PI*z,q) / (M_PI*theta1prime_(0.0,q));
	}

	static cpx e1_ (cpx q) {
		return M_PI*M_PI * (pow(theta2_(0,q),4) + 2.0 * pow(theta4_(0,q),4)) / 3.0;
	}

	static cpx zeta_ (cpx z, cpx q) {
		return 2.0 * z * eta1_(q) + M_PI * theta1prime_(M_PI*z,q) / theta1_(M_PI*z,q);
	}

	static cpx wp_ (cpx z, cpx q) {
		return e1_(q) + pow(M_PI*theta3_(0,q)*theta4_(0,q)*theta2_(M_PI*z,q)/theta1_(M_PI*z,q) ,2);
	}

	static cpx g2_ (cpx q) {
		cpx a=theta2_(0,q), b=theta3_(0,q);
		return pow(M_PI,4)/(.75)*(pow(a,8)-pow(a,4)*pow(b,4)+pow(b,8));
	}

	static cpx g3_ (cpx q) {
		cpx a=theta2_(0,q), b=theta3_(0,q);
		return pow(M_PI,6)/(pow(1.5,3))*(pow(a,12)-1.5*(pow(a,8)*pow(b,4)+pow(a,4)*pow(b,8))+pow(b,12));
	}
}
