#include <vb/Bitmap.h>
#include <vb/ProgressBar.h>

using namespace std;
using namespace vb;

vector<int> bridge (int n, bool p=false) {
	vector<int> v (n,-1);
	for (int i=0; i<n/2; ++i) v[i]=1;
	for (int i=n-1; i>=0; --i) swap (v[i], v[prng.uniform_int(i+1)]);

	if (!p) return v;

	int s=0, ms=0, mi=0;
	for (int i=0; i<n; ++i) {
		s += v[i];
		if (s<ms) { ms=s; mi=i; }
	}

	vector<int> w;
	for (int i=mi+1; i<n; ++i) w.push_back (v[i]);
	for (int i=0; i<=mi; ++i)  w.push_back (v[i]);
	return w;
}

int m=1;

class Stat {
public:
	Stat (int ss) : s(ss) { m = max(m,s); }
	operator int () { return s; }
	operator Color() {
		if (s==0) return WHITE;
		if (s==1) return RED;
		return Color(215 - (s*215)/m);
	}
	int s;
};

class Snake : public Bitmap<Stat> {
public:
	Snake (int n_) : Bitmap<Stat> (6*n_,6*n_), n(n_) {
		p.emplace_back(3*n,3*n);
		put (p.back(),1); show();
	};
	void grow (int d) {
		p.push_back (p.back() + dz[d]);
		put (p.back(), at(p.back()) + 1);
	}
	void shrink() { p.pop_back(); }

	int n;
	vector<coo> p;
};

int main (int argc, char ** argv) {
	H.init ("Random walk hull", argc,argv,"n=50,i,v");
	int64_t n = H['n'], l = n*n*n*n;
	bool inf = H['i'], vid = H['v'];

	Snake S(n);

	if (inf) {
		if (vid) S.snapshot_setup ("RWSH",1.0);
		while (norm(S.p.back() - coo(3*n,3*n)) < (3*n-1)*(3*n-1)) {
			if ((S.p.size()==1) || (prng.bernoulli(.5)))	S.grow(prng.uniform_int(4));
			else                                        	S.shrink();
		}
	} else {
		vector<int> b = bridge (l,true);
		for (int i=0; i<l; ++i) {
			if (b[i]==1)	S.grow(prng.uniform_int(4));
			else        	S.shrink();
		}
	}

	S.pause();
}
