#include <vb/Coloring.h>
#include <vb/Hub.h>
#include <vb/PRNG.h>
#include <boost/math/special_functions/binomial.hpp>

using namespace vb; using namespace std;

double sm3 (int i, int j, int k) {
	if (i<j) return sm3(j,i,k);
	if (i<k) return sm3(k,i,j);
	if (i==0) return 1.0;
	return sqrt(double(i+j+k)/double(i)/(9.0*M_PI)) * sm3 (i-1,j,k);
}

class RPoly { public:
	class Coef { public: int i, j, k; double a, b; };

	RPoly (int n) {
		map<string,function<double()>> generators;
		generators.emplace ("gaussian",  [](){ return prng.gaussian(0,1); });
		generators.emplace ("bernoulli", [](){ return prng.bernoulli(.5) ? 1 : -1; });

		for (int i=0; i<=n; ++i)
			for (int j=0; j<=n-i; ++j) {
				double a = generators[H['g']]();
				double b = a * sm3(i,j,n-i-j);
				coefs.push_back ({i,j,n-i-j,a,b});
			}
	}

	double operator() (double x, double y) {
		double out = 0;
		for (auto c : coefs) out += c.b * pow(x,c.i) * pow(y,c.j);
		return out;
	}

	Color operator() (cpx z) {
		return HSV ( (*this)(real(z),imag(z))>0 ? 0 : .5, .8, .8);
	}

	vector<Coef> coefs;
};

int main (int argc, char ** argv) {
	H.init ("Random polynomial in 2 variables", argc, argv, "n=100,g=gaussian,s=0");
	int s = H['s']; if (s) prng.seed(s);
	RPoly P (H['n']); Coloring C (cpx(-10,-10),cpx(10,10),800,P);
	C.show(); while (C.visible()) { C.update(); Fl::wait(); } C.output();
}
