#include <vb/Hub.h>
#include <vb/math.h>
#include <cln/cln.h>
#include <chrono>

using namespace vb; using namespace std; using namespace cln; using cln::complex;

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
}
