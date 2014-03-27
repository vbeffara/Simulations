#include <vb/Hub.h>
#include <vb/CoarseImage.h>
#include <vb/Console.h>
#include <vb/QuadTree.h>

using namespace vb; using namespace std;

class DLA : public CoarseImage { public:
	DLA (Hub & H) : CoarseImage(H['n'],H['n'], H.title, pow(H['n'],.33)), n(H['n']), r(1), deg(H['d']), QT(coo(-n/2,-n/2),coo(n/2,n/2),30) {
		z0 = coo(n/2,n/2);
		if (H['l']) for (int i=-n/4; i<n/4; ++i) put(i);
		W.watch (QT.n, "Nb of particles"); W.watch (r, "Cluster radius");
		prec.push_back (vector<double>()); prec.push_back (vector<double>());
		for (int d=2; d<50; ++d) {
			cerr << ".";
			int l=2*d-1;
			vector<vector<double>> M (l,vector<double>(l,0)); M[l/2][l/2]=1;
			bool dirty=1; while (dirty) { dirty=false;
				for (int i=1; i<l-1; ++i) for (int j=1; j<l-1; ++j) if ((i!=d-1)||(j!=d-1)) {
					double t = (M[i][j+1] + M[i][j-1] + M[i+1][j] + M[i-1][j])/4;
					if (t != M[i][j]) { M[i][j]=t; dirty=true;}
				}
			}
			vector<double> D1 = M[1];
			double s1=0; for (int i=0; i<l; ++i) s1 += D1[i]; for (int i=0; i<l; ++i) D1[i] /= s1;
			prec.push_back (D1);
		}
		cerr << endl;
	};

	void show    	()           	{ W.show(); CoarseImage::show(); }
	char at      	(coo z) const	{ return contains(z) && CoarseImage::at(z); }
	void put     	(coo z)      	{ CoarseImage::put(z,1); QT.insert(z); r = std::max (r,sup(z)); if (!(QT.n%1000)) { update(); W.update(); } }
	bool neighbor	(coo z) const	{ for (int i=0; i<deg; ++i) if (at(z+dz[i])) return true; return false; }

	coo jump (int d) const {
		if (d<=2) return dz[prng.uniform_int(deg)];
		if (d<prec.size()) {
			coo w (d-1, prng.discrete(prec[d]) - (d-1));
			if (prng.bernoulli()) w.x = - w.x;
			if (prng.bernoulli()) swap (w.x,w.y);
			return w;
		}
		int l = d - prec.size()/2;
		double theta = prng.uniform_real(0,2*M_PI), x = l*cos(theta), y = l*sin(theta);
		if (deg==6) { x += y/sqrt(3.0); y *= 2.0/sqrt(3.0); }
		return coo (x,y);
	}

	void runDLA () {
		put(0);
		while (r<n/2-1) {
			double t = prng.uniform_real(0,2*M_PI);
			coo z ((2*r+20)*cos(t), (2*r+20)*sin(t));
			QuadIndex qi { 0, sup(z) };
			while (!neighbor(z)) {
				qi.d = sup (z-qi.z); QT.nn (z,qi);
				z += jump(qi.d);
				if (sup(z) > 100*r) { z.x *= .9; z.y *= .9; }
			}
			put(z);
		}
	}

	int n,r,deg;
	Console W;
	QuadTree QT;
	vector<vector<double>> prec;
};

int main (int argc, char ** argv) {
	Hub H ("Lattice DLA",argc,argv,"n=500,l,d=4");
	DLA dla (H); dla.show(); dla.runDLA(); dla.output();
	cerr << "Final cluster: " << dla.QT.n << " particles, diameter = " << dla.r << endl;
}
