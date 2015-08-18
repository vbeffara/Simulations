#include <vb/Bitmap.h>
#include <vb/Console.h>
#include <vb/Hub.h>

using namespace vb;

class Site { public:
	Site (int d_ = 0, int x_ = 0, int k_ = 0) : d(d_), x(x_), k(k_) {}

	operator Color () { static std::vector<Color> key = { BLACK, RED, GREEN }; return key[d]; }

	int d, x, k;
};

class Nematic : public vb::Bitmap<Site> { public:
	Nematic (int n_, int k_, double b_) : Bitmap<Site> (n_,n_), k(k_), b(b_) {};

	bool fits	(coo z, int d, int k) { for (int i=0; i<k; ++i) { if (atp(z).d > 0) return false; z += dz[d-1]; } return true; }
	void add 	(coo z, int d, int k) { (d==1 ? nh : nv) ++; for (int i=0; i<k; ++i) { putp (z, {d,i,k}); z += dz[d-1]; } }
	void del 	(coo z, int d, int k) { (d==1 ? nh : nv) --; for (int i=0; i<k; ++i) { putp (z, {0,0,0}); z += dz[d-1]; } }

	void go () {
		Console C;
		C.watch (nh, "horizontal");  	C.watch (nv, "vertical");
		C.watch (density, "density");	C.watch (order, "order");
		C.watch (b, "beta");         	C.manage (b,-5.0,5.0);
		C.watch (k, "k");            	C.manage (k,1,100);
		show(); C.show(); if (H['v']) snapshot_setup("movie",10);
		while (true) {
			if (prng.bernoulli( exp(b) / (exp(b)+exp(-b)) )) {
				coo z = rand(); int d = prng.bernoulli(.5) + 1, kk=k; if (fits (z,d,kk)) add (z,d,kk);
			} else {
				coo z = rand(); if ((at(z).d > 0) && (at(z).x == 0)) del (z,at(z).d,at(z).k);
			}
			density = double ((nh+nv)*k) / (w()*h());
			order = nh+nv>0 ? double (nh-nv) / double (nh+nv) : 0;
		}
	};

	int k, nh=0, nv=0;
	double b, density=0, order=0;
};

int main (int argc, char ** argv) {
	H.init ("Nematic system on the square lattice", argc,argv, "n=400,k=10,b=0,v");
	Nematic(H['n'],H['k'],H['b']).go();
}
