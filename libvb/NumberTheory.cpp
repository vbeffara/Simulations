// #include <vb/impl/NumberTheory.hxx>
#include <vb/NumberTheory.h>
#include <boost/lexical_cast.hpp>
#include <fplll.h>

#ifdef FPLLL4
#define lll_reduction lllReduction
#define shortest_vector shortestVector
#endif

namespace vb {
	// template Polynomial<bigint> guess (real_t x, real_t eps);
	// template Polynomial<bigint> guess (real_t x, int leps, unsigned d);
	// template Polynomial<cpxint> guess (complex_t x, real_t eps);
	// template Polynomial<cpxint> guess (complex_t x, int leps, unsigned d);

	using namespace cln;

	boost::optional<cl_UP_R> guess (const cl_R & x, int nd) {
		cl_F xf = cl_float(x);
		auto m = expt (cl_float(10,xf),nd*2/3);

		for (int d=1; d<=nd/10; ++d) {
			auto t = cl_float (1,xf);
			ZZ_mat<mpz_t> M (d+1,d+2);

			for (int i=0; i<=d; ++i) {
				M[i][0].set_str(boost::lexical_cast<string> (round1(t*m)).c_str());
				M[i][i+1] = 1;
				t *= xf;
			}

			lll_reduction(M); vector<Z_NR<mpz_t>> o; shortest_vector(M,o);

			vector<cl_I> V (d+1,0);
			for (int j=0; j<d+1; ++j) {
				auto ai = boost::lexical_cast<cl_I> (o[j]);
				if (ai!=0) for (int i=0; i<=d; ++i) V[i] += ai * boost::lexical_cast<cl_I> (M[j][i+1]);
			}

			auto P = find_univpoly_ring (cl_R_ring, cl_symbol("z")) -> create (d);
			for (int i=0; i<=d; ++i) set_coeff (P, i, V[i]);
			finalize (P); if (V[d]<0) P=-P;

			auto PP = deriv (P); cl_F xx=xf, ox=xf+1; while (abs(xx-ox) > expt(cl_float(10),5-nd)) { ox = xx; xx -= P(xx)/PP(xx); }
			if (abs(xx-xf) < expt(cl_float(10,xf),5-nd)) return P;
		}

		return boost::none;
	}

	boost::optional<cl_UP_N> guess_c (const cl_N & x, int nd) {
		auto m = expt (cl_float(10),nd*2/3);

		for (int d=1; d<=nd/10; ++d) {
			cl_N t = cl_float (1, the<cl_F> (realpart(x)));
			ZZ_mat<mpz_t> M (2*(d+1),2*(d+2));

			for (int i=0; i<=d; ++i) {
				auto re = boost::lexical_cast<std::string> (round1(realpart(t)*m));
				auto im = boost::lexical_cast<std::string> (round1(imagpart(t)*m));
				auto mim = boost::lexical_cast<std::string> (round1(-imagpart(t)*m));
				M[2*i][0].set_str(re.c_str());
				M[2*i][1].set_str(im.c_str());
				M[2*i+1][0].set_str(mim.c_str());
				M[2*i+1][1].set_str(re.c_str());
				M[2*i][2*i+2] = 1;
				M[2*i+1][2*i+3] = 1;
				t *= x;
			}

			lll_reduction(M); vector<Z_NR<mpz_t>> o; shortest_vector(M,o);

			vector<cl_I> V (2*(d+1),0);
			for (int j=0; j<2*(d+1); ++j) {
				auto ai = boost::lexical_cast<cl_I> (o[j]);
				if (ai!=0) for (int i=0; i<2*(d+1); ++i) V[i] += ai * boost::lexical_cast<cl_I> (M[j][i+2]);
			}

			auto P = find_univpoly_ring (cl_C_ring, cl_symbol("z")) -> create (d);
			for (int i=0; i<=d; ++i) set_coeff (P, i, cln::complex(V[2*i],V[2*i+1]));
			finalize (P);
			if (realpart(coeff(P,d)) == 0) P = cln::complex(0,1) * P;
			if (realpart(coeff(P,d))<0) P=-P;

			auto PP = deriv (P); cl_N xx=x, ox=x+1; while (abs(xx-ox) > expt(cl_float(10),5-nd)) { ox = xx; xx -= P(xx)/PP(xx); }
			if (abs(xx-x) < expt(cl_float(10),10-nd)) return P;
		}

		return boost::none;
	}

	boost::optional<cl_UP_N> guess_r (const cl_N & x, int nd) {
		auto m = expt (cl_float(10),nd*2/3);

		for (int d=1; d<=nd/10; ++d) {
			cl_N t = cl_float (1, the<cl_F> (realpart(x)));
			ZZ_mat<mpz_t> M (d+1,d+3);

			for (int i=0; i<=d; ++i) {
				auto re = boost::lexical_cast<std::string> (round1(realpart(t)*m));
				auto im = boost::lexical_cast<std::string> (round1(imagpart(t)*m));
				M[i][0].set_str(re.c_str());
				M[i][1].set_str(im.c_str());
				M[i][i+2] = 1;
				t *= x;
			}

			lll_reduction(M); vector<Z_NR<mpz_t>> o; shortest_vector(M,o);

			vector<cl_I> V (2*(d+1),0);
			for (int j=0; j<d+1; ++j) {
				auto ai = boost::lexical_cast<cl_I> (o[j]);
				if (ai!=0) for (int i=0; i<d+1; ++i) V[i] += ai * boost::lexical_cast<cl_I> (M[j][i+2]);
			}

			auto P = find_univpoly_ring (cl_C_ring, cl_symbol("z")) -> create (d);
			for (int i=0; i<=d; ++i) set_coeff (P, i, V[i]);
			finalize (P); if (realpart(coeff(P,d))<0) P=-P; if (coeff(P,d)==0) continue;

			auto PP = deriv (P); cl_N xx=x, ox=x+1; while (abs(xx-ox) > expt(cl_float(10),5-nd)) { ox = xx; xx -= P(xx)/PP(xx); }
			if (abs(xx-x) < expt(cl_float(10),10-nd)) return P;
		}

		return boost::none;
	}
}
