#include <vb/Cube.h>
#include <vb/PRNG.h>
#include <vb/CL_Parser.h>
#include <vb/ProgressBar.h>

using namespace vb; using namespace std;

class Ising3 : public Cube <unsigned char, int> { public: int b; double beta; vector<double> glaub,kaw;
	Ising3 (CL_Parser & CLP) : Cube <unsigned char, int> (CLP('n'), CLP('n'), CLP('n'), "The 3d Ising model"), b(0), beta(CLP('b')) {
		for (int k=0; k<=6; ++k) glaub.push_back(exp(k*beta) / (exp(k*beta) + exp((6-k)*beta)));
		for (int k=0; k<=12; ++k) kaw.push_back(1/(1+exp(2*beta*(k-6))));
	};

	void bc_bernoulli (double p=.5) {
		for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) put(coo3(x,y,z), prng.bernoulli(p) ? 255 : 0);
	}

	void bc_dobrushin () {
		b=1; for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) put(coo3(x,y,z), y<sy/2 ? 255 : 0);
	}

	void bc_cube () {
		for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) put(coo3(x,y,z), 0);
		for (int x=sx/10; x<9*sx/10; ++x) for (int y=sy/10; y<9*sy/10; ++y) for (int z=sz/10; z<9*sz/10; ++z) put(coo3(x,y,z), 255);
	}

	int nbsum (coo3 c) { int S=0; for (int i=0; i<6; ++i) S += atp(c+dz3[i]) ? 1 : 0; return S; }

	void swipe_kawazaki () {
		for (int t=0; t<sx*sy*sz; ++t) {
			int d = prng.uniform_int(6); coo3 z1 = rand(b), z2 = z1+dz3[d]; if (at(z1) != atp(z2)) {
				if (prng.bernoulli(kaw[nbsum(z2)-nbsum(z1)+6])) { putp(z1,255); putp(z2,0); } else { putp(z1,0); putp(z2,255); }
			}
		}
	}

	void swipe_glauber () { for (int t=0; t<sx*sy*sz; ++t) { coo3 c = rand(b); put (c, prng.bernoulli(glaub[nbsum(c)]) ? 255 : 0); } }
};

int main (int argc, char ** argv) {
	CL_Parser CLP (argc, argv, "n=50,b=1,t=0");
	int T = CLP('t'); if (T==0) T = CLP('n');
    Ising3 C (CLP); C.bc_bernoulli(.2); C.show();
    ProgressBar P (T);

    for (int t=0; t<T; ++t) C.swipe_kawazaki(), P.set(t);
	C.output_pov();
}
