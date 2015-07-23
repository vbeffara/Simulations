#include <vb/Bitmap.h>
#include <vb/Hub.h>

using namespace vb; using namespace std;

enum Type { RIGHT=0, TOP=1, LEFT=2, DOWN=3, VOID, SITE, EDGE };

class Point { public:
	Point (int i=0) {}
	Point (Type _t) : t(_t) {}
	operator Color() {
		if (t==SITE) return WHITE;
		if (t==EDGE) return WHITE;
		if (int(t)<4) return Indexed(t);
		return BLACK;
	}

	Type t = VOID;
};

class UST : public Bitmap<Point> { public:
	UST (int n) : Bitmap<Point> (2*n+1,2*n+1) {}

	void lerw (coo z0) {
		coo z = z0;

		while (at(z).t != SITE) {
			int d = prng.uniform_int(4);
			put (z, Type(d));
			if (contains(z + dz[d])) z += dz[d] * 2;
		}

		z = z0;

		while (at(z).t != SITE) {
			int d = at(z).t;
			put (z, SITE);
			put (z+dz[d],EDGE);
			z += dz[d] * 2;
		}
	}
};

int main (int argc, char ** argv) {
	H.init ("Uniform spanning tree", argc, argv, "n=100");
	int n=H['n'];

	UST I (n);
	I.put (coo(0,2*(n/2)), SITE);
	I.show();

	for (int i=0; i<=n; ++i) for (int j=0; j<=n; ++j) I.lerw(coo(2*i,2*j));

	I.output();
}
