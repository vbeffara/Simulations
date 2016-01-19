#pragma once
#include <vb/cpx.h>

namespace vb {
	template <typename T> int sign (T x) { return (T(0) < x) - (x < T(0)); }

	static       	unsigned long long fact   	(unsigned long long n)                      	{ return (n<2 ? 1 : n*fact(n-1));  	}
	static inline	unsigned long long binom  	(unsigned long long n, unsigned long long k)	{ return fact(n)/fact(k)/fact(n-k);	}
	static inline	unsigned long long catalan	(unsigned long long n)                      	{ return binom(2*n,n)/(n+1);       	}

	template <typename T> T pi_ () { static T pi = T(4)*atan(T(1)); return pi; }

	template <typename T> std::complex<T> q_ (const std::complex<T> & tau) { return exp(std::complex<T>(0,pi_<T>())*tau); }

	template <typename T> std::complex<T> q_t (const std::complex<T> & tau) { return std::complex<T>(0,pi_<T>()) * exp(std::complex<T>(0,pi_<T>())*tau); }
}
