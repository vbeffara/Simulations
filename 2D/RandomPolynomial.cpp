#include <vb/Coloring.h>
#include <vb/Hub.h>
#include <vb/PRNG.h>

using namespace vb; using namespace std;

double sqfact (int i) {
	double out = 1;
	for (int k=2; k<=i; ++k) out *= sqrt(k);
	return out;
}

class RPoly { public:
	class Coef { public: int i, j; double a; };

	RPoly (int n) {
		map<string,function<double()>> generators;
		generators.emplace ("gaussian",  [](){ return prng.gaussian(0,1); });
		generators.emplace ("bernoulli", [](){ return prng.bernoulli(.5) ? 1 : -1; });

		for (int i=0; i<=n; ++i)
			for (int j=0; j<=n-i; ++j) {
				double v = generators[H['g']]();
				v *= sqfact(n)/(sqfact(i)*sqfact(j)*sqfact(n-i-j));
				coefs.push_back ({i,j,v});
			}
	}

	double operator() (double x, double y) {
		double out = 0;
		for (auto c : coefs) out += c.a * pow(x,c.i) * pow(y,c.j);
		return out;
	}

	Color operator() (cpx z) {
		return HSV ( (*this)(real(z),imag(z))>0 ? 0 : .5, .8, .8);
	}

	vector<Coef> coefs;
};

int main (int argc, char ** argv) {
	H.init ("Random polynomial in 2 variables", argc, argv, "n=10,a,g=gaussian,s=0");
	int s = H['s']; if (s) prng.seed(s);
	RPoly P (H['n']); Coloring C (cpx(-10,-10),cpx(10,10),800,P); C.aa = H['a'];
	C.show(); while (C.visible()) { C.update(); Fl::wait(); } C.output();
}
