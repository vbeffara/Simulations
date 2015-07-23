#include <vb/Bitmap.h>
#include <vb/Hub.h>
#include <vb/ProgressBar.h>

using namespace vb; using namespace std;

enum Type { VOID, SITE, EDGE, DUAL, DEDG, EMPH };

class Point { public:
	Point (int i=0) {}
	Point (Type _t) : t(_t) {}
	operator Color() {
		if (t == SITE) return ((d==0)||(d==1)) ? RED : Color(200,200,255);
		if (t == EDGE) return ((d==0)||(d==1)) ? RED : Color(200,200,255);

		if (t == DUAL) return ((d==2)||(d==3)) ? Color(200,100,0) : Color(128,255,128);
		if (t == DEDG) return ((d==2)||(d==3)) ? Color(200,100,0) : Color(128,255,128);

		if (t==EMPH) return BLACK;

		if (d<0) return WHITE;
		return Indexed(d);
	}

	int d = -1;
	Type t = VOID;
};

class UST : public Bitmap<Point> { public:
	UST (int n_) : Bitmap<Point> (2*n_,2*n_+1), n(n_) {}

	void lerw (coo z0, bool killed = false) {
		coo z = z0;
		while (at(z).t != SITE) {
			step();
			int d = prng.uniform_int(4); at(z).d = d;
			if (((d==0)||(d==1)) && prng.bernoulli(1-double(H['a'])*double(H['a']))) continue;
			if (contains(z + dz[d] * 2)) { z += dz[d] * 2; continue; }
			if (killed && (d==2)) break;
			if (d==2) z=z0;
		}
		z = z0;
		while (contains(z) && (at(z).t != SITE)) { int d = at(z).d; at(z).t = SITE; put (z+dz[d],EDGE); at(z+dz[d]).d = d; z += dz[d] * 2; }
	}

	void path (coo z) {
		while (at(z).t != EMPH) { at(z).t = EMPH; at(z + dz[at(z).d]).t = EMPH; z += dz[at(z).d] * 2; }
	}

	void dual () {
		bool dirty = true; while (dirty) {
			dirty = false;
			for (int i=0; i<n; ++i) for (int j=0; j<n; ++j) {
				coo z (2*i,1+2*j);
				int s=0, dd=-1;
				for (int d=0; d<4; ++d) if (at(z + dz[d]).t == VOID) { ++s; dd=d; }
				if (s==1) {
					at(z) = DUAL;
					at(z).d = dd;
					at(z + dz[dd]) = DEDG;
					at(z + dz[dd]).d = dd;
					dirty = true;
				}
			}
		}
	}

	void special () {
		double aa = double(H['a']); aa=aa*aa;

		Array<double> P (n,n+1,0); P.at(coo(n-1,n/2)) = 1;
		{ ProgressBar PB (10*n*n);
			for (int t=0; t<10*n*n; ++t) {
				PB.set(t);
				// cerr << ".";
				for (int i=0; i<n; ++i) for (int j=0; j<n+1; ++j) {
					if ((i==n-1) && (j==n/2)) continue;
					coo z (i,j);
					double s=0;
					for (int d=0; d<4; ++d) if (P.contains(z + dz[d])) {
						if ((d==0)||(d==1)) s += aa * P.at(z+dz[d]);
						else s += P.at(z+dz[d]);
					}
					P.at(z) = s / (2 + 2*aa);
				}
			}
		}

		coo z0 (0,n/4);

		// while (P.contains(z)) {
		//	if (z == coo(n-1,n/2)) break;
		//	at(coo(1+2*z.x,2*z.y)).d=0; step();
		//	z += dz[d];
		// }

		coo z = z0;
		while (z != coo(n-1,n/2)) {
			vector<double> pp (4,0);
			for (int d=0; d<4; ++d) if (P.contains(z+dz[d])) pp[d] = P.at(z+dz[d]);
			double s=0; for (auto p : pp) s += p; for (auto &p : pp) p /= s;
			int d = prng.discrete(pp);

			coo tz (1+2*z.x,2*z.y);
			at(tz).d = d;
			z += dz[d];
		}
		z = z0;
		while (z != coo(n-1,n/2)) {
			coo tz (1+2*z.x,2*z.y);
			int d = at(tz).d;
			at(tz).t = SITE; put (tz+dz[d],EDGE); at(tz+dz[d]).d = d;
			z += dz[d];
		}
	}

	int n;
};

int main (int argc, char ** argv) {
	H.init ("2-periodic uniform spanning forest", argc, argv, "n=300,a=1");
	int n=H['n'];

	UST I (n);

	coo root (2*n-1,2*(n/2)), start (1,2*(n/4));

	I.put (root, SITE);

	I.show();
	I.special();

	// I.lerw(start, false);
	for (int i=0; i<n; ++i) for (int j=0; j<=n; ++j) I.lerw(coo(2*i+1,2*j), true);

	I.dual();

	I.put (root, EMPH); I.path (start);

	I.pause(); I.output();
}
