#include <vb/Image.h>

using namespace vb; using namespace std;

#define STATE_NONE    0
#define STATE_PRESENT 128
#define STATE_NE      1
#define STATE_FLIP    2
#define STATE_VISITED 4

#define MIRROR_NONE           	Color(0)
#define MIRROR_NE             	Color(255,0,0)
#define MIRROR_NW             	Color(255,255,0)
#define MIRROR_FLIP_NE        	Color(0,0,255)
#define MIRROR_FLIP_NW        	Color(0,255,0)
#define MIRROR_NE_VISITED     	Color(128,0,0)
#define MIRROR_NW_VISITED     	Color(128,128,0)
#define MIRROR_FLIP_NE_VISITED	Color(0,0,128)
#define MIRROR_FLIP_NW_VISITED	Color(0,128,0)

const vector<Color> colors = {	MIRROR_NW, MIRROR_NE, MIRROR_FLIP_NW, MIRROR_FLIP_NE,
                              	MIRROR_NW_VISITED, MIRROR_NE_VISITED, MIRROR_FLIP_NW_VISITED, MIRROR_FLIP_NE_VISITED };

const vector<int> flip_ne = {1,0,3,2}, flip_nw = {3,2,1,0};

class Mirror {
public:
	Mirror (uint8_t s_ = 0) : s(s_) {}
	uint8_t s;
	operator uint8_t () { return s; }
	operator Color () {
		if (s==0) return BLACK;
		if (s==STATE_VISITED) return Grey(128);
		return colors[s%8];
	}
};

class Mirrors : public Bitmap<Mirror> {
public:
	Mirrors (Hub & H) :	Bitmap<Mirror> (H['n'],H['n']),
	                   	p(H['p']), q(H['q']), f(H['f']) {}
	void main ();
	double p, q, f;
};

void Mirrors::main () {
	show();

	while (true) {
		for (Mirror & m : *this) {
			m = STATE_NONE;
			if (prng.bernoulli(1-q)) {
				m.s |= STATE_PRESENT;
				if (prng.bernoulli(p)) m.s |= STATE_NE;
				if (prng.bernoulli(f)) m.s |= STATE_FLIP;
			}
		}

		coo z(w()/2,h()/2);
		for (int t=0, d=0; (t<8*w()*h()) && contains(z); ++t) {
			if (at(z) & STATE_PRESENT) {
				if (at(z).s & STATE_NE) d = flip_ne[d]; else d = flip_nw[d];
				if (at(z).s & STATE_FLIP) at(z).s ^= STATE_NE;
			}
			at(z).s |= STATE_VISITED; z += dz[d];
		}

		pause();
	}
}

int main (int argc, char ** argv) { H.init ("Mirror model", argc, argv, "n=200,p=.5,q=0,f=0"); Mirrors(H).main(); }
