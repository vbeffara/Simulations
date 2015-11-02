#include <vb/Bitmap.h>
#include <vb/Hub.h>

using namespace vb; using namespace std;

class Wave : public Bitmap<Color> { public:
	Wave (int n, int k, double l_) : Bitmap<Color> (n,n), l(l_) {
		for (int i=0; i<k; ++i) {
			a.push_back(prng.gaussian());
			th.push_back(prng.uniform_real(0,2.0*M_PI));
		}
	}

	cpx v (cpx z) {
		cpx out = 0;
		if (H['r']) z -= cpx(0,h()/2);
		for (int i=0; i<a.size(); ++i) {
			out += a[i] * exp(I * (z.real()*cos(th[i]) + z.imag()*sin(th[i])) / l);
			if (H['r']) out += a[i] * exp(I * (z.real()*cos(th[i]) - z.imag()*sin(th[i])) / l);
		}
		return out;
	}

	Color c (cpx z) { return v(z).real() > 0 ? RED : BLUE; }

	void go () {
		show();
		std::function<Color(cpx)> f = [&](cpx z) { return c(z); };
		tessel(0,{w()-1,h()-1},aa(f)); pause();
	};

	vector<double> a,th;
	double l;
};

int main (int argc, char ** argv) {
	H.init ("Random planar waves", argc, argv, "n=500,k=30,l=5,r");
	Wave(H['n'],H['k'],H['l']).go();
}
