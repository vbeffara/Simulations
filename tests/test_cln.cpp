#include <vb/Hub.h>
#include <vb/LinearAlgebra.h>
#include <vb/NumberTheory.h>
#include <boost/optional.hpp>
#include <chrono>

using namespace vb; using namespace std; using namespace cln;

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
	cl_F lll2 ("0.9162918442410306144165008200767499077603397502333144975769802641182380808885019256331544308341889255");
	cout << "CLN 100: " << lll2 << endl;
	if (auto P2 = guess (lll2, 100)) cout << "  root of " << *P2 << endl;
	if (auto P3 = guess_r (lll2, 100)) cout << "  root of " << *P3 << endl;

	cout << endl;
	cpx100 zzz (gmp100("0.1722882583776278670500267959231284336682007863854856624427574750255049273322927690638923632"),
		gmp100("0.5302487364574217190358808797265653491226567421626168710631761419479819886565504921987031543"));
	cout << "GMP 100: " << zzz << endl;
	Polynomial<cpxint> PPP = guess (zzz, gmp100(1e-80));
	if (PPP.degree()>0) cout << "  root of " << PPP << endl;

	cl_F zzz1 ("0.1722882583776278670500267959231284336682007863854856624427574750255049273322927690638923632");
	cl_F zzz2 ("0.5302487364574217190358808797265653491226567421626168710631761419479819886565504921987031543");
	cl_N zzzc = cln::complex (zzz1,zzz2);
	cout << "CLN 100: " << zzzc << endl;
	if (auto PPP2 = guess_c (zzzc, 100)) cout << "  root of " << *PPP2 << endl;
	if (auto PPP3 = guess_r (zzzc, 100)) cout << "  root of " << *PPP3 << endl;

	cout << endl;
	Matrix<cl_N> m (3,3); for (int i=0; i<3; ++i) for (int j=0; j<3; ++j) m(i,j) = int(pow(2*i+1,j));
	cout << "M = "; printmath (cout,m); cout << endl;
	Vector<cl_N> v (3);   for (int i=0; i<3; ++i) v(i) = int(3*i-2);
	cout << "V = "; printmath (cout,v); cout << endl;
	cout << "M^{-1}.V = "; printmath (cout, solve(m,v)); cout << endl;
}
