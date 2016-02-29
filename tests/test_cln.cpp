#include <vb/Elliptic.h>
#include <vb/Hub.h>
#include <vb/math.h>
#include <cln/cln.h>
#include <chrono>

using namespace vb; using namespace std; using namespace cln; using cln::complex;

cl_N sum (std::function <cl_N(int)> f) {
	cl_N out (0), old (1);
	for (int n=0; out!=old; ++n) {
		old = out; out += f(n);
	}
	return out;
}

cl_N q_ (const cl_N & tau) { return exp(cln::complex(0,pi()) * tau); }

cl_N pow (const cl_N & a, const cl_N & b) { return exp(b*log(a)); }

cl_N theta1 (const cl_N & q, const cl_N & z) {
	cl_N q14 = exp(log(q)/4);
	return sum ([&](int n) {
		return 2 * q14 * pow(cl_N(-1),n) * pow(q, n*(n+1)) * sin((2*n+1)*z);
	});
}

double time () {
	static std::chrono::system_clock C;
	static auto basetime = C.now();
	chrono::duration<double> dur = C.now() - basetime;
	return dur.count();
}

int main (int argc, char ** argv) {
	H.init ("Testing numerical types", argc,argv, "n=100000");
	int n=H['n'];
	double t=0;

	cout << setprecision(100);
	default_float_format = float_format(100);

	cout << "sin(1)" << endl;
	cout << "  gmp100:      " << sin(gmp100(1)) << endl;
	cout << "  CLN(100):    " << sin(cl_float(1)) << endl;
	cout << "  Mathematica: 0.8414709848078965066525023216302989996225630607983710656727517099919104043912396689486397435430526959" << endl;
	cout << endl;
	cout << "cos(\\pi/10)" << endl;
	cout << "  gmp100:      " << cos(pi_<gmp100>()/10) << endl;
	cout << "  CLN(100):    " << cos(pi()/10) << endl;
	cout << "  Mathematica: 0.9510565162951535721164393333793821434056986341257502224473056444301531700851935017187928109708113817" << endl;
	cout << endl;
	cout << "q_(.1) = exp(i \\pi / 10)" << endl;
	cout << "  cpx100:     " << q_(cpx100(1)/gmp100(10)) << endl;
	cout << "  CLN(100):    " << q_(cl_float(1)/10) << endl;
	cout << "  Mathematica: 0.9510565162951535721164393333793821434056986341257502224473056444301531700851935017187928109708113817 +"
						<< "0.3090169943749474241022934171828190588601545899028814310677243113526302314094512248536036020946955687 I" << endl;
	cout << endl;

	cout << "double:     time = "; t = time();
	double s = 0; for (int i=0; i<n; ++i) s = cos(s);
	cout << setprecision(10) << time()-t << ", \tresult = " << setprecision(100) << s << endl;

	cout << "CLN short:  time = "; t = time();
	auto cs = cl_F("0.0s0"); for (int i=0; i<n; ++i) cs = cos(cs);
	cout << setprecision(10) << time()-t << ", \tresult = " << setprecision(100) << cs << endl;

	cout << "CLN single: time = "; t = time();
	auto c1 = cl_F("0.0f0"); for (int i=0; i<n; ++i) c1 = cos(c1);
	cout << setprecision(10) << time()-t << ", \tresult = " << setprecision(100) << c1 << endl;

	cout << "CLN double: time = "; t = time();
	auto c2 = cl_F("0.0d0"); for (int i=0; i<n; ++i) c2 = cos(c2);
	cout << setprecision(10) << time()-t << ", \tresult = " << setprecision(100) << c2 << endl;

	cout << "CLN float:  time = "; t = time();
	auto c = cl_float(0.0); for (int i=0; i<n; ++i) c = cos(c);
	cout << setprecision(10) << time()-t << ", \tresult = " << setprecision(100) << c << endl;

	cout << "gmp100:     time = "; t = time();
	gmp100 g (0); for (int i=0; i<n; ++i) g = cos(g);
	cout << setprecision(10) << time()-t << ", \tresult = " << setprecision(100) << g << endl;

	cout << "CLN 100:    time = "; t = time();
	auto cc = cl_float(0); for (int i=0; i<n; ++i) cc = cos(cc);
	cout << setprecision(10) << time()-t << ", \tresult = " << setprecision(100) << cc << endl;

	cout << endl;

	cpx100 tau(.125,1.25), q(q_(tau)), z(.25,.75); Elliptic<gmp100> E { q };
	cout << tau << endl;
	cout << q << endl;
	cout << z << endl;
	cout << E.theta1(z) << endl;

	cout << endl;

	auto tau2 = cln::complex(1,10)/8, q2 = q_(tau2), z2 = cln::complex(1,3)/4;
	cout << tau2 << endl;
	cout << q2 << endl;
	cout << z2 << endl;
	cout << theta1(q2,z2) << endl;
}
