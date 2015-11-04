#include <vb/Bitmap.h>
#include <vb/Hub.h>

using namespace vb; using namespace std;

class mode { public:
	mode (double a_, double t_, double l_) : a(a_), c(cos(t_)/l_), s(sin(t_)/l_), r(H['r']) {}
	double operator() (const cpx & z) { double out = cos(c*real(z) + s*imag(z)); if (r) out += cos(c*real(z) - s*imag(z)); return a * out; }
	double a,c,s;
	bool r;
};

class Wave : public Bitmap<Color> { public:
	Wave (int n, int k, double l_) : Bitmap<Color> (n,n) {
		for (int i=0; i<k; ++i) {
			m.push_back ( { prng.gaussian(), prng.uniform_real(0,2.0*M_PI), l_ } );
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
	H.init ("Random planar waves", argc, argv, "n=600,k=100,l=5,r");
	Wave(H['n'],H['k'],H['l']).go();
}
