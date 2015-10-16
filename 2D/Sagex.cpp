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
		int out = 1 + prng.uniform_int(3); if ((out==2) && (state==1)) out = 0; return dz[out];
	}
};

class Sagex : public Bitmap<Site> {
public:
	Sagex (int n) : Bitmap<Site> (n,n) {
		for (auto & s : *this) if (prng.bernoulli(H['l'])) s = prng.bernoulli(H['p']) ? 1 : 2;
	}

	void go () {
		show(); if (H['v']) snapshot_setup("Sagex",1);
		while (true) {
			coo z = this->rand(), dz = at(z).jump();
			if ((int(at(z))!=0) && (int(atp(z+dz))==0)) { putp(z+dz,int(at(z))); put(z,0); }
		}
	}
};

int main (int argc, char ** argv) {
	H.init ("Seb's Sagex process", argc,argv, "n=400,l=.8,p=.5,v,z");
	Sagex (H['n']).go();
}
