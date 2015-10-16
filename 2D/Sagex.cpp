#include "vb/Bitmap.h"
#include "vb/Hub.h"

using namespace vb; using namespace std;

const Color C[4] = { BLACK, RED, GREEN };

class Site {
	int state; // 0 empty, 1 right, 2 left
public:
	Site (int i) : state(i) {};
	operator int()   const { return state; }
	operator Color() const { return C[state]; }
	coo jump() {
		if (H['z']) { return dz[prng.uniform_int(2) + (state==2 ? 2 : 0)]; }
		int out = prng.uniform_int(4); if (out + 2*state == 4) out = 2-out; return dz[out];
	}
};

class Sagex : public Bitmap<Site> {
public:
	Sagex (int n) : Bitmap<Site> (n,n) {
		for (auto & s : *this) if (prng.bernoulli(H['l'])) s = prng.bernoulli(H['p']) ? 1 : 2;
	}

	void go () {
		show(); if (H['v']) snapshot_setup("Sagex",1);
		for (int t=H['t']; t!=1; --t) {
			coo z = this->rand(); if (int(at(z))==0) continue;
			coo dz = at(z).jump(); if (int(atp(z+dz))==0) { putp(z+dz,int(at(z))); put(z,0); }
		}
	}
};

int main (int argc, char ** argv) {
	H.init ("Seb's Sagex process", argc,argv, "n=512,l=.22,p=.5,v,z,t=0");
	Sagex (H['n']).go();
}
