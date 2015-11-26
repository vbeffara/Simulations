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
		if (z) { return dz[prng.uniform_int(2) + (state==2 ? 2 : 0)]; }
		int out = prng.uniform_int(4); if (out + 2*state == 4) out = 2-out; return dz[out];
	}
	bool z;
};

class Sagex : public Bitmap<Site> {
public:
	Sagex (int w, int h) : Bitmap<Site> (w,h) {
		for (auto & s : *this) { if (prng.bernoulli(H['l'])) s = prng.bernoulli(H['p']) ? 1 : 2; s.z = H['z']; }
	}

	void go () {
		show(); if (H['v']) snapshot_setup("Sagex",1);
		while (true) {
			coo z = this->rand(); if (int(at(z))==0) continue;
			coo dz = at(z).jump(); if (int(atp(z+dz))==0) { putp(z+dz, at(z)); put(z,0); }
		}
	}
};

int main (int argc, char ** argv) {
	H.init ("Seb's Sagex process", argc,argv, "n=400,w=0,l=.22,p=.5,v,z");
	Sagex (int(H['w']) ? H['w'] : H['n'], H['n']).go();
}
