#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/Image.h>

using namespace vb; using namespace std;

vector<Color> C { WHITE, RED, BLACK };

class TASEP : public vector<int> {
	public:
		TASEP (CL_Parser & CLP) : vector<int>(int(CLP('n')),0), p(size(),1) {
			for (int & e : *this) { e = 2*prng.bernoulli(CLP('r')); if (prng.bernoulli(CLP('d')) && (e==2)) e = 1; }
			for (double & q : p) q = prng.uniform_real(CLP('e'),1.0000001);
		}
		void step () {
			int i = prng.uniform_int(size()); if (prng.bernoulli(1-p[i])) return;
			if (at(i) > at((i+1)%size())) { std::swap(at(i),at((i+1)%size())); }
		}

		vector<double> p;
};

int main (int argc, char ** argv) {
	CL_Parser CLP (argc,argv,"n=1280,r=.4,e=0,d=0"); TASEP T(CLP);

	Image I (T.size(),700,"TASEP"); I.show();
	for (int t=0,u=0 ;; ++t,++u) {
		for (int j=0; j<u; ++j) for (unsigned i=0; i<T.size(); ++i) T.step();
		for (unsigned i=0; i<T.size(); ++i) I.put(coo(i,t%I.h()),C[T[i]]);
		if (t==I.h()) I.pause();
	}
}