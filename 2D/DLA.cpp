#include <vb/Hub.h>
#include <vb/CoarseImage.h>
#include <vb/Console.h>
#include <vb/QuadTree.h>

using namespace vb;
using namespace std;

int l1 (coo z) { return abs(z.x) + abs(z.y); }

class DLA : public CoarseImage { public:
	DLA (Hub & H) : CoarseImage(H['n'],H['n'], H.title, pow(H['n'],.33)), n(H['n']), r(1), deg(H['d']), QT(coo(-n/2,-n/2),coo(n/2,n/2),30) {
		z0 = coo(n/2,n/2);
		if (H['l']) for (int i=-n/4; i<n/4; ++i) put(i);
		W.watch (QT.n, "Nb of particles");
		W.watch (r,    "Cluster radius");
	};

	void show    	()           	{ W.show(); CoarseImage::show(); }
	char at      	(coo z) const	{ return contains(z) && CoarseImage::at(z); }
	void put     	(coo z)      	{ CoarseImage::put(z,1); r = max (r, l1(z)); QT.insert(z); }
	bool neighbor	(coo z) const	{ for (int i=0; i<deg; ++i) if (at(z+dz[i])) return true; return false; }

	coo jump (int l) const {
		double theta = prng.uniform_real(0,2*M_PI), x = l*cos(theta), y = l*sin(theta);
		if (deg==6) { x += y/sqrt(3.0); y *= 2.0/sqrt(3.0); }
		return coo (x,y);
	}

	int dist (coo z, int d) const { return QT.dist(z); }

	void runDLA () {
		put(0);
		while (r<n/2-1) {
			coo z = jump (2*r+2);
			int d = r+2;
			while (!neighbor(z)) {
				d = dist(z,d);
				if (d<10) {
					z += dz[prng()%deg];
				} else {
					z += jump(d/3);
					if (l1(z) > 100*r) {
						z.x *= .9;
						z.y *= .9;
					}
				}
				d = min (2*d, l1(z));
			}
			put(z);
		}
	}

	int n,r,deg;
	Console W;
	QuadTree QT;
};

int main (int argc, char ** argv) {
	Hub H ("Lattice DLA",argc,argv,"n=1000,l,d=4");
	DLA dla (H); dla.show(); dla.runDLA(); dla.output();
}
