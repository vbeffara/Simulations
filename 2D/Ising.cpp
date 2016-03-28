#include <vb/Image.h>

using namespace vb; using namespace std;

const Color LEFT(BLACK), RIGHT(WHITE);
const double betac = log(1+sqrt(2));

class Ising : public Image { public:
	Ising (int nn, double bb, double rr, bool cc) : Image(nn,nn), n(nn), beta(bb) {
		if (rr) { for (auto & i : *this) i = prng.bernoulli(rr) ? LEFT : RIGHT; }
		else { for (int x=1; x<n-1; x++) for (int y=1; y<n-1; y++) put(coo(x,y), x<(n>>1) ? LEFT : RIGHT); }

		if (cc) { c=1;
			for (int i=0; i<(n>>1); i++)	{ put(coo(i,0),LEFT);   	put(coo(i,n-1),LEFT); 	}
			for (int i=(n>>1); i<n; i++)	{ put(coo(i,0),RIGHT);  	put(coo(i,n-1),RIGHT);	}
			for (int i=0; i<n; i++)     	{ put(coo(0,i),LEFT);   	                      	}
			for (int i=0; i<n; i++)     	{ put(coo(n-1,i),RIGHT);	                      	}
		}
	}

	int nnb (coo z, Color c = 128) {
		if (c==Color(128)) c = atp(z); int out=0;
		for (int d=0; d<4; ++d) { if (atp(z+dz[d]) == c) out++; else out--; }
		return out;
	}

	void run (int nstep) {
		vector<double> p(10,0); for (int i=0; i<10; ++i) p[i] = exp(-i*beta);
		if (!nstep) nstep = 10 + n * 0.01 / fabs(beta-betac);

		for (int i=0; i!=nstep; i++) {
			for (int x=c; x<n-c; x++) {
				for (int y=c; y<n-c; y++) {
					if (H['k']) {
						coo z = rand(), zz = z + dz[prng.uniform_int(4)]; if (atp(z) == atp(zz)) continue;
						int s = nnb(z) + nnb(zz) + 2;
						if ( (s<=0) || (prng.bernoulli(p[s])) ) { Color c = atp(z); putp(z, atp(zz)); putp(zz, c); }
					} else {
						coo z(x,y); int s=nnb(z);
						if ( (s<=0) || (prng.bernoulli(p[s])) ) put(coo(x,y),(255-at(z)));
					}
				}
			}
		}
	}

	int n,c=0;
	double beta;
};

int main(int argc, char *argv[]) {
	H.init ("Ising model", argc,argv, "b=.88137359,n=300,t=0,c,r=0,k");
	Ising I (H['n'], H['b'], H['r'], H['c']); I.show(); I.run(H['t']); I.output();
}
