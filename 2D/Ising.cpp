#include <vb/Image.h>

using namespace vb; using namespace std;

class Ising : public Image { public:
	Ising (int nn, double bb, double rr, bool cc) : Image(nn,nn), n(nn), c(cc), beta(bb) {
		if (rr) { for (auto & i : *this) i = prng.bernoulli(rr) ? BLACK : WHITE; }
		else { for (int x=1; x<n-1; x++) for (int y=1; y<n-1; y++) put(coo(x,y), x<(n>>1) ? BLACK : WHITE); }
		if (c) for (int i=0; i<n; i++) {
			put (coo(i,0), i<n/2 ? BLACK : WHITE);
			put (coo(i,n-1), i<n/2 ? BLACK : WHITE);
			put (coo(0,i),BLACK); put (coo(n-1,i),WHITE);
		}
	}

	int nnb (coo z) {
		int out = 0; Color c = atp(z);
		for (int d=0; d<4; ++d) { if (atp(z+dz[d]) == c) out++; else out--; }
		return out;
	}

	void run (int nstep, bool k) {
		vector<double> p(10,0); for (int i=0; i<10; ++i) p[i] = exp(-i*beta);
		if (!nstep) nstep = 10 + n * 0.01 / fabs(beta-log(1+sqrt(2)));

		for (int i=0; i!=nstep; i++) for (int j=0; j<n*n; ++j) {
			step(); coo z = rand(c);
			if (k) {
				coo zz = z + dz[prng.uniform_int(4)]; if (c && !contains (zz,c)) continue; if (atp(z) == atp(zz)) continue;
				int s = nnb(z) + nnb(zz) + 2;
				if ( (s<=0) || (prng.bernoulli(p[s])) ) { swap (atp(z), atp(zz)); }
			} else {
				int s = nnb(z);
				if ( (s<=0) || (prng.bernoulli(p[s])) ) put(z,(255-at(z)));
			}
		}
	}

	int n,c=0;
	double beta;
};

int main(int argc, char *argv[]) {
	H.init ("Ising model", argc,argv, "b=.88137359,n=300,t=0,c,r=0,k,v");
	Ising I (H['n'], H['b'], H['r'], H['c']); I.show(); if (H['v']) I.snapshot_setup("Ising",10);
	I.run(H['t'],H['k']); I.output();
}
