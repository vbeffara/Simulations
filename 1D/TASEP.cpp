#include <vb/Image.h>

using namespace vb; using namespace std;

vector<Color> C { WHITE, RED, BLACK };

class TASEP : public vector<int> {
	public:
		TASEP () : vector<int>(int(H['n']),0), p(size(),1) {
			for (int & e : *this) { e = 2*prng.bernoulli(H['r']); if (prng.bernoulli(H['d']) && (e==2)) e = 1; }
			for (double & q : p) q = prng.uniform_real(H['e'],1.0000001);
		}
		void step () {
			int i = prng.uniform_int(size()); if (prng.bernoulli(1-p[i])) return;
			if (at(i) > at((i+1)%size())) { std::swap(at(i),at((i+1)%size())); }
		}

		vector<double> p;
};

int main (int argc, char ** argv) {
	H.init ("TASEP", argc,argv, "n=1280,r=.4,e=0,d=0");

	TASEP T; Image I (T.size(),700); I.show();
	for (int t=0,u=0 ;; ++t,++u) {
		for (int j=0; j<u; ++j) for (unsigned i=0; i<T.size(); ++i) T.step();
		for (unsigned i=0; i<T.size(); ++i) I.put(coo(i,t%I.h()),C[T[i]]);
		if (t==I.h()-1) I.pause();
	}
}
