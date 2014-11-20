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

	template <typename T> T pi_ () { static T pi = T(4)*atan(T(1)); return pi; }

	template <typename T> std::complex<T> q_ (const std::complex<T> & tau) { return exp(std::complex<T>(0,pi_<T>())*tau); }

	template <typename T> std::complex<T> q_t (const std::complex<T> & tau) { return std::complex<T>(0,pi_<T>()) * exp(std::complex<T>(0,pi_<T>())*tau); }

	template <typename T> std::complex<T> theta1_ (const std::complex<T> & z, const std::complex<T> & q) {
		std::complex<T> out(0), old(1);
		for (int n=0; out!=old; ++n) { old=out; out += T(2) * pow(T(-1),n) * pow(q, (n+.5)*(n+.5)) * sin(T(2*n+1)*z); }
		return out;
	}

	template <typename T> std::complex<T> theta1_z (const std::complex<T> & z, const std::complex<T> & q) {
		std::complex<T> out(0), old(1);
		for (int n=0; out!=old; ++n) { old=out; out += T(2) * pow(T(-1),n) * pow(q, (n+.5)*(n+.5)) * T(2*n+1) * cos(T(2*n+1)*z); }
		return out;
	}

	template <typename T> std::complex<T> theta1_q (const std::complex<T> & z, const std::complex<T> & q) {
		std::complex<T> out(0), old(1);
		for (int n=0; out!=old; ++n) { old=out; out += T(2) * pow(T(-1),n) * T((n+.5)*(n+.5)) * pow(q, (n+.5)*(n+.5)-1) * sin(T(2*n+1)*z); }
		return out;
	}

	template <typename T> std::complex<T> theta1_zq (const std::complex<T> & z, const std::complex<T> & q) {
		std::complex<T> out(0), old(1);
		for (int n=0; out!=old; ++n) { old=out; out += T(2) * pow(T(-1),n) * T((n+.5)*(n+.5)) * pow(q, (n+.5)*(n+.5)-1) * T(2*n+1) * cos(T(2*n+1)*z); }
		return out;
	}

	template <typename T> std::complex<T> theta1_zz (std::complex<T> z, std::complex<T> q) {
		std::complex<T> out(0), old(1);
		for (int n=0; out!=old; ++n) { old=out; out -= T(2) * pow(-1,n) * pow(q, (n+.5)*(n+.5)) * pow(T(2*n+1),2) * sin(T(2*n+1)*z); }
		return out;
	}

	template <typename T> std::complex<T> theta1_zzz (const std::complex<T> & z, const std::complex<T> & q) {
		std::complex<T> out(0), old(1);
		for (int n=0; out!=old; ++n) { old=out; out -= T(2) * pow(T(-1),n) * pow(q, (n+.5)*(n+.5)) * pow(T(2*n+1),3) * cos(T(2*n+1)*z); }
		return out;
	}

	template <typename T> std::complex<T> theta1_zzzq (const std::complex<T> & z, const std::complex<T> & q) {
		std::complex<T> out(0), old(1);
		for (int n=0; out!=old; ++n) { old=out; out -= T(2) * pow(T(-1),n) * T((n+.5)*(n+.5)) * pow(q, (n+.5)*(n+.5)-1) * pow(T(2*n+1),3) * cos(T(2*n+1)*z); }
		return out;
	}

	template <typename T> std::complex<T> theta2_ (std::complex<T> z, std::complex<T> q) {
		std::complex<T> out(0), old(1);
		for (int n=0; out!=old; ++n) { old=out; out += T(2) * pow(q, (n+.5)*(n+.5)) * cos(T(2*n+1)*z); }
		return out;
	}

	template <typename T> std::complex<T> theta2_z (std::complex<T> z, std::complex<T> q) {
		std::complex<T> out(0), old(1);
		for (int n=0; out!=old; ++n) { old=out; out -= T(2) * pow(q, (n+.5)*(n+.5)) * T(2*n+1) * sin(T(2*n+1)*z); }
		return out;
	}

	template <typename T> std::complex<T> theta2_q (std::complex<T> z, std::complex<T> q) {
		std::complex<T> out(0), old(1);
		for (int n=0; out!=old; ++n) { old=out; out += T(2) * T((n+.5)*(n+.5)) * pow(q, (n+.5)*(n+.5)-1) * cos(T(2*n+1)*z); }
		return out;
	}

	template <typename T> std::complex<T> theta3_ (std::complex<T> z, std::complex<T> q) {
		std::complex<T> out(1), old(2);
		for (int n=1; out!=old; ++n) { old=out; out += T(2) * pow(q, n*n) * cos(T(2*n)*z); }
		return out;
	}

	template <typename T> std::complex<T> theta3_q (std::complex<T> z, std::complex<T> q) {
		std::complex<T> out(0), old(1);
		for (int n=1; out!=old; ++n) { old=out; out += T(2) * T(n*n) * pow(q, n*n-1) * cos(T(2*n)*z); }
		return out;
	}

	template <typename T> std::complex<T> theta4_ (std::complex<T> z, std::complex<T> q) {
		std::complex<T> out(1), old(2);
		for (int n=1; out!=old; ++n) { old=out; out += T(2) * pow(-q, n*n) * cos(T(2*n)*z); }
		return out;
	}

	template <typename T> std::complex<T> theta4_q (std::complex<T> z, std::complex<T> q) {
		std::complex<T> out(0), old(1);
		for (int n=1; out!=old; ++n) { old=out; out -= T(2) * T(n*n) * pow(-q, n*n-1) * cos(T(2*n)*z); }
		return out;
	}

	template <typename T> std::complex<T> eta1_ (const std::complex<T> & q) {
		return - pi_<T>()*pi_<T>() * theta1_zzz(std::complex<T>(0),q) / theta1_z(std::complex<T>(0),q) / T(6);
	}

	template <typename T> std::complex<T> eta1_q (const std::complex<T> & q) {
		return - pi_<T>()*pi_<T>()/T(6) * (theta1_z(std::complex<T>(0),q)*theta1_zzzq(std::complex<T>(0),q) - theta1_zzz(std::complex<T>(0),q)*theta1_zq(std::complex<T>(0),q)) / pow(theta1_z(std::complex<T>(0),q),2);
	}

	template <typename T> std::complex<T> sigma_ (const std::complex<T> & z, const std::complex<T> & q) {
		return exp(eta1_(q)*z*z) * theta1_(pi_<T>()*z,q) / (pi_<T>()*theta1prime_(0.0,q));
	}

	template <typename T> std::complex<T> e1_ (std::complex<T> q) {
		return pi_<T>()*pi_<T>() * (pow(theta2_(std::complex<T>(0),q),4) + T(2) * pow(theta4_(std::complex<T>(0),q),4)) / T(3);
	}

	template <typename T> std::complex<T> zeta_ (std::complex<T> z, std::complex<T> q) {
		return T(2) * z * eta1_(q) + pi_<T>() * theta1prime_(pi_<T>()*z,q) / theta1_(pi_<T>()*z,q);
	}

	template <typename T> std::complex<T> wp_ (std::complex<T> z, std::complex<T> q) {
		return e1_(q) + pow(pi_<T>()*theta3_(std::complex<T>(0),q)*theta4_(std::complex<T>(0),q)*theta2_(pi_<T>()*z,q)/theta1_(pi_<T>()*z,q) ,2);
	}

	template <typename T> std::complex<T> g2_ (std::complex<T> q) {
		std::complex<T> a=theta2_(0,q), b=theta3_(0,q);
		return pow(pi_<T>(),4)/(.75)*(pow(a,8)-pow(a,4)*pow(b,4)+pow(b,8));
	}

	template <typename T> std::complex<T> g3_ (std::complex<T> q) {
		std::complex<T> a=theta2_(0,q), b=theta3_(0,q);
		return pow(pi_<T>(),6)/(pow(1.5,3))*(pow(a,12)-1.5*(pow(a,8)*pow(b,4)+pow(a,4)*pow(b,8))+pow(b,12));
	}
}
