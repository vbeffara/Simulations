#pragma once

namespace vb {
	unsigned long long fact   	(unsigned long long n)                      	{ return (n<2 ? 1 : n*fact(n-1));     	}
	unsigned long long binom  	(unsigned long long n, unsigned long long k)	{ return fact(n)/fact(k)/fact(n-k);   	}
	unsigned long long catalan	(unsigned long long n)                      	{ return binom(2*n,n)/(n+1);          	}
}