#pragma once
#include <vb/Polynomial.h>
#include <fplll.h>

namespace vb {
	using bigint = boost::multiprecision::number<boost::multiprecision::gmp_int>;

	template <typename T> Polynomial<T> guess (T x, T eps);
	template <typename T> Polynomial<T> guess (T x, int leps, unsigned d);

	template <typename T> Polynomial<complex<T>> guess (complex<T> x, T eps);
	template <typename T> Polynomial<complex<T>> guess (complex<T> x, int leps, unsigned d);

	/***************************************************************************************/

	template <typename T> Polynomial<T> guess (T x, T eps) {
		int leps (T(-log10(eps))); assert (leps>20);
		Polynomial<T> P;
		for (unsigned d=2; d<=10; ++d) { P = guess(x,leps,d); if ((P.degree()>1) || (P[1]!=1)) break; }
		return P;
	}

	template <typename T> Polynomial<complex<T>> guess (complex<T> x, T eps) {
		int leps (T(-log10(eps))); assert (leps>20);
		Polynomial<complex<T>> P;
		for (unsigned d=2; d<=10; ++d) { P = guess(x,leps,d); if ((P.degree()>1) || (P[1]!=complex<T>(1))) break; }
		return P;
	}

	template <typename T> Polynomial<T> guess (T x, int leps, unsigned d) {
		unsigned ndig = leps-12; bigint m = pow(bigint(10),ndig);
		T t(1);
		ZZ_mat<mpz_t> M(d+1, d+2);

		for (unsigned i=0; i<=d; ++i) { M[i][0].set(bigint(t*m).backend().data()); M[i][i+1].set(1); t *= x; }
		lllReduction(M);
		vector<Z_NR<mpz_t>> o; shortestVector(M,o);

		Polynomial<T> P(vector<T>(d+1));

		for (unsigned j=0; j<d+1; ++j) {
			bigint ai (o[j].getData());
			if (ai!=0) for (unsigned i=0; i<=d; ++i) P[i] += ai * bigint(M[j][i+1].getData());
		}

		Polynomial<T> PP = P.derivative();
		T xx=x, ox=x+1; while (abs(xx-ox) > pow(T(.1),ndig+20)) { ox = xx; xx -= P(xx)/PP(xx); }

		if (abs(x-xx)*pow(T(10),ndig) > 1e-10) P = Polynomial<T> {{-x,1}};
		return P;
	}

	template <typename T> Polynomial<complex<T>> guess (complex<T> x, int leps, unsigned d) {
		unsigned ndig = leps-12; bigint m = pow(bigint(10),ndig);
		complex<T> t(1);
		ZZ_mat<mpz_t> M(2*(d+1), 2*(d+2));

		for (unsigned i=0; i<=d; ++i) {
			M[2*i][0].set(bigint(real(t)*m).backend().data());
			M[2*i+1][1].set(bigint(real(t)*m).backend().data());
			M[2*i][1].set(bigint(imag(t)*m).backend().data());
			M[2*i+1][0].set(bigint(imag(-t)*m).backend().data());
			M[2*i][2*i+2].set(1); M[2*i+1][2*i+3].set(1);
			t *= x;
		}
		lllReduction(M);
		vector<Z_NR<mpz_t>> o; shortestVector(M,o);

		vector<T> tmp(2*(d+1));
		for (unsigned j=0; j<2*(d+1); ++j) {
			bigint ai (o[j].getData());
			if (ai!=0) for (unsigned i=0; i<2*(d+1); ++i) tmp[i] += ai * bigint(M[j][i+2].getData());
		}

		Polynomial<complex<T>> P(vector<complex<T>>(d+1)); for (unsigned i=0; i<=d; ++i) P[i] = complex<T> (tmp[2*i],tmp[2*i+1]);

		Polynomial<complex<T>> PP = P.derivative();
		complex<T> xx=x, ox=x+complex<T>(1); while (abs(xx-ox) > pow(T(.1),ndig+20)) {
			if (P(xx)==complex<T>(0)) break;
			ox = xx; xx -= P(xx)/PP(xx);
		}

		if (abs(x-xx)*pow(T(10),ndig) > 1e-10) P = Polynomial<complex<T>> {{-x,complex<T>(1)}};
		return P;
	}
}
