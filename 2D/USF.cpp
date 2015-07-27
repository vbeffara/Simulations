#include <vb/Bitmap.h>
#include <vb/Hub.h>

using namespace vb; using namespace std;

enum Type { VOID, SITE, EDGE, DUAL, DEDG, EMPH };

class Point { public:
	Point (int i=0) {}
	Point (Type t_, int d_ = 0) : t(t_), d(d_) {}
	operator Color() {
		if (t == SITE) return ((d==0)||(d==1)) ? RED : Color(200,200,255);
		if (t == EDGE) return ((d==0)||(d==1)) ? RED : Color(200,200,255);

		if (t == DUAL) return ((d==2)||(d==3)) ? Color(200,100,0) : Color(128,255,128);
		if (t == DEDG) return ((d==2)||(d==3)) ? Color(200,100,0) : Color(128,255,128);

		if (t==EMPH) return BLACK;

		if (d<0) return WHITE;
		return Indexed(d);
	}

	Type t = VOID;
	int d = -1;
};

class USF : public Bitmap<Point> { public:
	USF (int n_, double a_) : Bitmap<Point> (2*n_,2*n_+1), n(n_), a(a_), aa(a*a), root (2*n-1,2*(n/2)), start (1,2*(n/4)) {}

	void lerw (coo z0, bool killed = false) {
		coo z = z0;
		while (at(z).t != SITE) {
			step();
			int d = prng.uniform_int(4); at(z).d = d;
			if (((d==0)||(d==1)) && prng.bernoulli(1-aa)) continue;
			if (contains(z + dz[d] * 2)) { z += dz[d] * 2; continue; }
			if (killed && (d==2)) break;
			if (d==2) z=z0;
		}
		z = z0;
		while (contains(z) && (at(z).t != SITE)) {
			int d = at(z).d; at(z).t = SITE; at(z+dz[d]) = { EDGE, d }; z += dz[d] * 2;
		}
	}

	void path (coo z) {
		while (at(z).t != EMPH) {
			int d = at(z).d; at(z).t = EMPH; at(z+dz[d]).t = EMPH; z += dz[d] * 2;
		}
	}

	void dual () {
		bool dirty = true; while (dirty) {
			dirty = false;
			for (int i=0; i<n; ++i) for (int j=0; j<n; ++j) {
				coo z (2*i,1+2*j);
				int s=0, dd=-1;
				for (int d=0; d<4; ++d) if (at(z + dz[d]).t == VOID) { ++s; dd=d; }
				if (s==1) {
					at(z) = { DUAL, dd };
					at(z + dz[dd]) = { DEDG, dd };
					dirty = true;
				}
			}
		}
	}

	void special () {
		double laa = log(2+2*aa);
		coo rootp { n-1, n/2 }, startp { 0, n/4 };

		Array<double> P (n,n+1,0);
		double per = 1, mer = .1;
		while (true) {
			double er = 0;
			#pragma omp parallel for reduction(+:er) schedule(static)
			for (int i=0; i<n; ++i) {
				vector<double> ps(4,0);
				for (int j=0; j<n+1; ++j) {
					coo z {i,j};
					if (z == rootp) continue;
					double minp = 1; int mind = -1;
					for (int d=0; d<4; ++d) if (P.contains(z+dz[d])) {
						ps[d] = P.at(z+dz[d]);
						if (ps[d] < minp) { minp = ps[d]; mind = d; }
					}
					double s (mind<2 ? aa : 1);
					for (int d=0; d<4; ++d) if ((d!=mind) && (P.contains(z+dz[d]))) {
						if (d<2) s += aa * exp(ps[d] - minp);
						else s += exp(ps[d] - minp);
					}
					double ns = log(s) + minp - laa;
					er += fabs (P.at(z) - ns);
					P.at(z) = ns;
				}
			}
			cerr << er << "           \r";
			if ((per == mer) && (er >= per)) break; else { mer = min (mer,er); per = er; }
		}
		cerr << "                                \r";

		coo z = startp;
		vector<double> pp (4,0);
		while (z != coo(n-1,n/2)) {
			for (int d=0; d<4; ++d) pp[d] = P.contains(z+dz[d]) ? P.at(z+dz[d]) : 0;
			double minp = 0; for (int d=0; d<4; ++d) minp = min (minp, pp[d]);
			for (int d=0; d<4; ++d) if (P.contains(z+dz[d])) pp[d] = (d<2 ? aa : 1) * exp(pp[d] - minp);
			double s=0; for (auto p : pp) s += p; for (auto &p : pp) p /= s;
			int d = prng.discrete(pp);
			at (coo (1+2*z.x,2*z.y)).d = d; z += dz[d];
		}
		z = startp;
		while (z != coo(n-1,n/2)) {
			coo tz (1+2*z.x,2*z.y); int d = at(tz).d;
			at(tz).t = SITE; at(tz+dz[d]) = { EDGE, d };
			z += dz[d];
		}
	}

	void go () {
		if (H['v']) show();

		put (root, SITE);
		if (double(H['a'])<1) special(); else lerw(start, false);
		for (int i=0; i<n; ++i) for (int j=0; j<=n; ++j) lerw(coo(2*i+1,2*j), true);

		if (H['v']) pause();
		put (root, EMPH); path (start);
		if (H['v']) pause();
		dual();
		if (H['v']) pause();
		output();
	}

	int n;
	double a, aa;
	coo root, start;
};

int main (int argc, char ** argv) {
	H.init ("2-periodic uniform spanning forest", argc, argv, "n=300,a=.5,v");
	USF(H['n'],H['a']).go();
}
