#pragma once
#include <vb/Polynomial.h>
#include <fplll.h>

namespace vb {
	template <typename T> Polynomial<bigint> guess (T x, T eps);
	template <typename T> Polynomial<bigint> guess (T x, int leps, unsigned d);

	template <typename T> Polynomial<cpxint> guess (complex<T> x, T eps);
	template <typename T> Polynomial<cpxint> guess (complex<T> x, int leps, unsigned d);

	/* Real-valued version **************************************************************************************/

	template <typename T> Polynomial<bigint> guess (T x, T eps) {
		int leps (T(-log10(eps))); assert (leps>20);
		Polynomial<bigint> P;
		for (unsigned d=1; d<=10; ++d) { P = guess(x,leps,d); if (P.degree()>0) break; }
		if (P[P.degree()]<0) P = bigint(-1) * P;
		return P;
	}

	template <typename T> Polynomial<bigint> guess (T x, int leps, unsigned d) {
		unsigned ndig = leps-12; bigint m = pow(bigint(10),ndig);
		T t(1);
		ZZ_mat<mpz_t> M(d+1, d+2);

		for (unsigned i=0; i<=d; ++i) { M[i][0].set(bigint(t*m).backend().data()); M[i][i+1].set(1); t *= x; }
		lllReduction(M);
		vector<Z_NR<mpz_t>> o; shortestVector(M,o);

		Polynomial<bigint> P(vector<bigint>(d+1));

		for (unsigned j=0; j<d+1; ++j) {
			bigint ai (o[j].getData());
			if (ai!=0) for (unsigned i=0; i<=d; ++i) P[i] += ai * bigint(M[j][i+1].getData());
		}

		Polynomial<bigint> PP = P.derivative();
		T xx=x, ox=x+1; while (abs(xx-ox) > pow(T(.1),ndig+20)) { ox = xx; xx -= P(xx)/PP(xx); }

		if (abs(x-xx)*pow(T(10),ndig) > 1e-10) P = Polynomial<bigint> {{0}};
		return P;
	}

	/* Complex-valued version **************************************************************************************/

	template <typename T> Polynomial<cpxint> guess (complex<T> x, T eps) {
		int leps (T(-log10(eps))); assert (leps>20);
		Polynomial<cpxint> P;
		for (unsigned d=1; d<=10; ++d) { P = guess(x,leps,d); if (P.degree()>0) break; }
		if (abs(imag(P[P.degree()])) > abs(real(P[P.degree()]))) P = cpxint(0,1) * P;
		if (real(P[P.degree()]) < 0) P = cpxint(-1) * P;
		return P;
	}

	template <typename T> Polynomial<cpxint> guess (complex<T> x, int leps, unsigned d) {
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

		vector<bigint> tmp(2*(d+1));
		for (unsigned j=0; j<2*(d+1); ++j) {
			bigint ai (o[j].getData());
			if (ai!=0) for (unsigned i=0; i<2*(d+1); ++i) tmp[i] += ai * bigint(M[j][i+2].getData());
		}

		Polynomial<cpxint> P(vector<cpxint>(d+1)); for (unsigned i=0; i<=d; ++i) P[i] = cpxint (tmp[2*i],tmp[2*i+1]);

		Polynomial<cpxint> PP = P.derivative();
		complex<T> xx=x, ox=x+complex<T>(1); while (abs(xx-ox) > pow(T(.1),ndig+20)) { ox = xx; xx -= P(xx)/PP(xx); }

		if (abs(x-xx)*pow(T(10),ndig) > 1e-10) P = std::vector<cpxint> {{0}};
		return P;
	}
}
