#include <vb/Hub.h>
#include <vb/NumberTheory.h>
#include <vb/Polynomial.h>
#include <vb/math.h>
#include <cln/cln.h>
#include <chrono>
#include <fplll.h>

using namespace vb; using namespace std; using namespace cln; using cln::complex;

cl_I bigint_to_cl_I (bigint z) { ostringstream os; os << z; cl_I out = os.str().c_str(); return out; }
bigint cl_I_to_bigint (cl_I z) { ostringstream os; os << z; return bigint(os.str()); }

namespace vb {
	template<> template<> cl_N Polynomial<bigint>::operator() (cl_N z) const {
		cl_N out = bigint_to_cl_I(back());
		for (unsigned i=size()-1; i>0; --i) out = out*z + bigint_to_cl_I(at(i-1));
		return out;
	}
}

Polynomial<bigint> guess2 (cl_N x, int leps, unsigned d) {
	unsigned ndig = leps-12; bigint m = pow(bigint(10),ndig); cl_I mm = the<cl_I> (expt(cl_I(10),ndig));
	cl_F t = cl_float(1);
	ZZ_mat<mpz_t> M(d+1, d+2);

	for (unsigned i=0; i<=d; ++i) {
		cl_I tm = round1(t*mm);
		bigint tm2 = cl_I_to_bigint(tm);
		M[i][0].set(tm2.backend().data());
		M[i][i+1].set(1);
		t *= x;
	}
	lllReduction(M);
	vector<Z_NR<mpz_t>> o; shortestVector(M,o);

	Polynomial<bigint> P(vector<bigint>(d+1));

	for (unsigned j=0; j<d+1; ++j) {
		bigint ai (o[j].getData());
		if (ai!=0) for (unsigned i=0; i<=d; ++i) P[i] += ai * bigint(M[j][i+1].getData());
	}

	Polynomial<bigint> PP = P.derivative();
	cl_N xx=x, ox=x+1; while (abs(xx-ox) > expt(cl_float(.1),ndig+20)) { ox = xx; xx -= P(xx)/PP(xx); }

	if (abs(x-xx)*expt(cl_float(10),ndig) > 1e-10) P = Polynomial<bigint> {{0}};
	return P;
}

Polynomial<bigint> guess2 (cl_N x, int leps) {
	Polynomial<bigint> P;
	assert (leps>20);
	for (int d=1; d<=leps/5; ++d) { P = guess2 (x,leps,d); if (P.degree()>0) break; }
	if (P[P.degree()]<0) P = bigint(-1) * P;
	return P;
}

template <typename T> T sum (std::function <T(int)> f) {
	T out (0), old (1);
	for (int n=0; out!=old; ++n) { old = out; out += f(n); }
	return out;
}

cl_N q_ (const cl_N & tau) { return exp(cln::complex(0,pi()) * tau); }

cl_N pow (const cl_N & a, int k) { return expt(a,k); }

template <typename T> T theta1 (const T & q, const T & z) {
	T q14 = exp(log(q)/T(4));
	return sum<T> ([&](int n) { return T(2) * q14 * pow(T(-1),n) * pow(q, n*(n+1)) * sin(T(2*n+1)*z); });
}

double time () {
	static std::chrono::system_clock C;
	static auto basetime = C.now();
	chrono::duration<double> dur = C.now() - basetime;
	return dur.count();
}

template <typename T> void test (string s, int n) {
	double t = time(); T z = 0;
	for (int i=0; i<n; ++i) z = cos(z);
	ostringstream os; os << time()-t;
	cout << s << " | time = " << os.str() << "\t | result = " << z << endl;
}

int main (int argc, char ** argv) {
	H.init ("Testing numerical types", argc,argv, "n=100000");
	int n=H['n'];

	default_float_format = float_format(100);
	cout << setprecision(100);

	test <float>       ("Standard float ", n);
	test <double>      ("Standard double", n);
	test <long double> ("Long double    ", n);
	test <cl_N>        ("CLN 100        ", n);
	test <gmp100>      ("GMP 100        ", n);

	cout << endl;
	cout << "sin(1)" << endl;
	cout << "  gmp100:      " << sin(gmp100(1)) << endl;
	cout << "  CLN(100):    " << sin(cl_float(1)) << endl;

	cout << endl;
	cout << "cos(\\pi/10)" << endl;
	cout << "  GMP 100: " << cos(pi_<gmp100>()/10) << endl;
	cout << "  CLN 100: " << cos(pi()/10) << endl;

	cout << endl;
	cout << "q_(.1) = exp(i \\pi / 10)" << endl;
	cout << "  GMP 100: " << q_(cpx100(1)/gmp100(10)) << endl;
	cout << "  CLN 100:  " << q_(cl_float(1)/10) << endl;

	cout << endl;
	cout << "theta1 ((1+10i)/8, (1+3i)/4)" << endl;
	cpx100 tau(.125,1.25), q(q_(tau)), z(.25,.75);
	auto tau2 = cln::complex(1,10)/8, q2 = q_(tau2), z2 = cln::complex(1,3)/4;
	cout << "  GML 100: " << theta1(q,z) << endl;
	cout << "  CLN 100:  " << theta1(q2,z2) << endl;

	cout << endl;
	gmp100 lll ("0.9162918442410306144165008200767499077603397502333144975769802641182380808885019256331544308341889255");
	cout << "GMP 100: " << lll << endl;
	Polynomial<bigint> P = guess (lll, gmp100(1e-80)); if (P.degree()>0) cout << "  root of " << P << endl;
	cl_N lll2 ("0.9162918442410306144165008200767499077603397502333144975769802641182380808885019256331544308341889255");
	cout << "CLN 100: " << lll2 << endl;
	Polynomial<bigint> P2 = guess2 (lll2, 80);
	if (P.degree()>0) cout << "  root of " << P2 << endl;
}
