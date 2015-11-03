#include <vb/Bitmap.h>
#include <vb/Hub.h>

using namespace vb; using namespace std;

class mode { public:
	mode (double a_, double t_, double l_) : a(a_), t(t_), l(l_), c(cos(t)), s(sin(t)) {}
	double operator() (cpx z) { return a * cos((real(z)*c+imag(z)*s) / l); }
	double a,t,l,c,s;
};

class Wave : public Bitmap<Color> { public:
	Wave (int n, int k, double l_) : Bitmap<Color> (n,n) {
		for (int i=0; i<k; ++i) {
			m.push_back ( { prng.gaussian(), prng.uniform_real(0,2.0*M_PI), l_ } );
			if (H['r']) m.push_back ( { m.back().a, - m.back().t, m.back().l } );
		}
	}

	double v (cpx z) { double out = 0; for (auto mm : m) out += mm(z); return out; }

	Color c (cpx z) { return Indexed (v(z) > 0 ? 1 : 2); }

	void go () {
		std::function<Color(cpx)> f = [&](cpx z) { return c (z - cpx(0,h()/2)); };
		show(); tessel(0,{w()-1,h()-1},aa(f)); pause();
	};

	vector<mode> m;
};

int main (int argc, char ** argv) {
	H.init ("Random planar waves", argc, argv, "n=500,k=30,l=5,r");
	Wave(H['n'],H['k'],H['l']).go();
}
