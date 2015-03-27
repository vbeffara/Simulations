#include <vb/Hub.h>
#include <vb/Image.h>

using namespace vb;

class Voter : public Image { public:
	Voter (int n, double p, int d) : Image (n,n,H.title) {
		for (auto & v : *this)
			if (d>1)	v = Indexed(prng.uniform_int(d));
			else    	v = prng.bernoulli(p) ? WHITE : BLACK;
	}

	void up () { coo z = rand(); put(z,atp(z+dz[prng.uniform_int(4)])); }
};

int main (int argc, char **argv) {
	H.init ("Voter model", argc,argv, "n=500,p=.5,d=1");

	Voter V (H['n'],H['p'],H['d']);

	V.show(); while (true) V.up();
}
