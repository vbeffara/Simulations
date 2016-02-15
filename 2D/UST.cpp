#include <vb/Bitmap.h>

using namespace vb; using namespace std;

enum Type { VOID, SITE, EDGE, EMPH };

class Point { public:
	Point (int i=0) {i=i;}
	Point (Type _t) : t(_t) {}
	operator Color() {
		if ((t == SITE) || (t == EDGE)) return WHITE;
		if (t == EMPH) return RED;
		if (d<0) return BLACK; else return Indexed(d);
	}

	int d = -1;
	Type t = VOID;
};

class UST : public Bitmap<Point> { public:
	UST (int n_) : Bitmap<Point> (2*n_+1,2*n_+1), n(n_) {}

	void path (coo z, Type tgt) {
		while (at(z).t != tgt) { coo d = dz[at(z).d]; at(z).t = at(z+d).t = tgt; z += d*2; }
	}

	void lerw (coo z0) {
		coo z = z0; while (at(z).t != SITE) {
			int d = prng.uniform_int(4); at(z).d = d;
			if (contains(z + dz[d])) z += dz[d] * 2;
			step();
		}
		path (z0,SITE);
	}

	void go () {
		show();
		put (coo(0,2*(n/2)), SITE);
		for (int i=n; i>=0; --i) for (int j=0; j<=n; ++j) lerw(coo(2*i,2*j));
		if (H['p']) { put (coo(0,2*(n/2)), EMPH); path (coo(2*n,2*(n/4)), EMPH); }
		pause(); output();
	}

	int n;
};

int main (int argc, char ** argv) {
	H.init ("Uniform spanning tree", argc, argv, "n=200,p");
	UST(H['n']).go();
}
