#include <vb/Coloring.h>
#include <vb/PRNG.h>

using namespace vb; using namespace std;

class Wave : public Coloring { public:
	class Mode { public:
		Mode (double a_, double t, double l, double p_) : a(a_), c(l*cos(t)), s(l*sin(t)), p(t) {}
		double operator() (const cpx & z) { return a * cos(c*real(z) + s*imag(z) + p); }
		double a,c,s,p;
	};

	Wave (int n, int k, double l_, double e_, double u_) : Coloring (cpx(-n,-n), cpx(n,n), n, [&](cpx z){return c(z);}) {
		for (int i=0; i<k; ++i) {
			double ll = (1 + e_ * prng.gaussian() + u_ * (2*prng.uniform_real()-1)) * l_;
			m.emplace_back (prng.gaussian(), prng.uniform_real(0,2.0*M_PI), ll, prng.uniform_real(0,2.0*M_PI));
		}
	}

	double v (cpx z) { double out = 0; for (auto mm : m) out += mm(z); return out; }

	Color c (cpx z) { return Indexed (v(z) > 0 ? 1 : 2); }

	vector<Mode> m;
};

int main (int argc, char ** argv) {
	H.init ("Random planar waves", argc, argv, "n=600,k=1000,l=.1,e=0,u=0");
	Wave W(H['n'],H['k'],H['l'],H['e'],H['u']); W.show(); W.output(); Fl::run();
}
