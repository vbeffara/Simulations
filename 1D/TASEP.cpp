#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/Image.h>

using namespace vb; using namespace std;

vector<Color> C { WHITE, RED, BLACK };

class TASEP : public vector<int> {
	public:
		TASEP (CL_Parser & CLP) : vector<int>(int(CLP('n')),0), p(size(),1) {
			for (int & e : *this) e = 2*prng.bernoulli(CLP('r')); 
		}
		void step () {
			int i = prng.uniform_int(size()); if (prng.bernoulli(1-p[i])) return;
			if (at(i) > at((i+1)%size())) { std::swap(at(i),at((i+1)%size())); }
		}

		vector<double> p;
};

int main (int argc, char ** argv) {
	CL_Parser CLP (argc,argv,"n=1200,r=.4"); TASEP T(CLP);
	for (double & p : T.p) p = prng.uniform_real();
	for (int & e : T) if (prng.bernoulli(.01)) e = 1;

	Image I (T.size(),700,"TASEP"); I.show();
	for (int t=0,u=0 ;; ++t) { T.step(); if (sqrt(2*t/T.size())>u) {
		u = sqrt(2*t/T.size());
		for (int i=0; i<T.size(); ++i) I.put(coo(i,u%I.h()),C[T[i]]); ++u; 
		if (u==I.h()) I.pause();
	} }
}