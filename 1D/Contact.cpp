#include <vb/Hub.h>
#include <vb/Bitmap.h>

using namespace vb;
using namespace std;

const vector<Color> C { WHITE, BLACK, GREEN };

class Site {
public:
	Site (int _s) : s(_s) {}
	operator Color() { return C[s]; }
	int s;
};

class Contact : public vector<Site> {
public:
	Contact (Hub & H) : vector<Site>(int(H['n']),0), lambda1(H['l']), lambda2(H['m']), total(1+lambda1+lambda2), jump(H['j']) {
		for (auto &s : (*this)) if (prng.bernoulli(H['p'])) s = 1;
		for (auto &s : (*this)) if (prng.bernoulli(H['e'])) s = 2;
	}
	void step () {
		int i = prng.uniform_int(size()), j = (i + (prng.bernoulli(.5)?1:size()-1)) % size();
		if (prng.bernoulli(1.0/(1.0+lambda1+lambda2))) { at(i)=0; return; }
		if (prng.bernoulli(lambda1/(lambda1+lambda2))) { if (at(i).s == 1) at(j)=1; return; }
		if (at(i).s != 2) return;
		if (at(j).s == 0) { at(j) = 2; return; }
		if (jump && (at(j).s == 1)) {
			int k = (i + 2*(j-i) + size()) % size();
			if (at(k).s == 0) at(k)=2;
		}
	}
	double lambda1,lambda2,total;
	bool jump;
};

int main (int argc, char ** argv) {
	Hub H ("1D contact process",argc,argv,"n=1000,p=1,e=0,l=3.3,m=100,j"); Contact T(H);

	Bitmap<Site> I (T.size(),700,H.title); I.show();
	for (int t=0 ;; ++t) {
		for (int i=0; i<I.w(); ++i) I.putp(coo(i,t),T[i]);
		for (int i=0; i<I.w() * T.total; ++i) T.step();
		if (t%I.h() == I.h()-1) I.pause();
	}
}
