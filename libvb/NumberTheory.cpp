#include <vb/impl/NumberTheory.hxx>
#include <vb/util.h>

namespace vb {
	template Polynomial<bigint> guess (gmp100 x, gmp100 eps);
	template Polynomial<bigint> guess (gmp100 x, int leps, unsigned d);
	template Polynomial<cpxint> guess (std::complex<gmp100> x, gmp100 eps);
	template Polynomial<cpxint> guess (std::complex<gmp100> x, int leps, unsigned d);

	using namespace cln;

	boost::optional<cl_UP_R> guess (cl_F x, int nd) {
		auto m = expt (cl_float(10,x),nd*2/3);

		for (int d=1; d<=nd/10; ++d) {
			auto t = cl_float (1,x);
			ZZ_mat<mpz_t> M (d+1,d+2);

			for (int i=0; i<=d; ++i) { ostringstream os; os << round1(t*m); M[i][0].set_str(os.str().c_str()); M[i][i+1] = 1; t *= x; }

			lllReduction(M); vector<Z_NR<mpz_t>> o; shortestVector(M,o);

			vector<cl_I> V (d+1,0);
			for (int j=0; j<d+1; ++j) {
				auto ai = reparse<cl_I> (o[j]);
				if (ai!=0) for (int i=0; i<=d; ++i) V[i] += ai * reparse<cl_I> (M[j][i+1]);
			}

			auto P = find_univpoly_ring (cl_R_ring, cl_symbol("z")) -> create (d);
			for (int i=0; i<=d; ++i) set_coeff (P, i, V[i]); finalize (P); if (V[d]<0) P=-P;

			auto PP = deriv (P); cl_F xx=x, ox=x+1; while (xx != ox) { ox = xx; xx -= P(xx)/PP(xx); }
			if (abs(1-xx/x) < expt(cl_float(10,x),5-nd)) return P;
		}

		return boost::none;
	}
}
