#include <vb/CoarseImage.h>
#include <vb/Console.h>
#include <vb/QuadTree.h>

using namespace vb; using namespace std;

class DLA : public CoarseImage { public:
	DLA (Hub & H) : CoarseImage(H['n'],H['n'], pow(double(H['n']),.33)),
		n(H['n']), c(H['c']), r(1), QT(coo(-n/2,-n/2),coo(n/2,n/2),H['l']),
		img (512,512) {
		z0 = coo(n/2,n/2);
		W.watch (QT.n, "Nb of particles"); W.watch (r, "Cluster radius");
		prec.emplace_back (vector<double>()); prec.emplace_back (vector<double>());
		for (int d=2; d<int(H['p']); ++d) {
			H.L->info ("Precomputing harmonic measure: d={}", d);
			int l=2*d-1;
			vector<vector<double>> M (l,vector<double>(l,0)); M[l/2][l/2]=1;
			bool dirty=true; while (dirty) { dirty=false;
				for (int i=1; i<l-1; ++i) for (int j=1; j<l-1; ++j) if ((i!=d-1)||(j!=d-1)) {
					double t = (M[i][j+1] + M[i][j-1] + M[i+1][j] + M[i-1][j])/4;
					if (t != M[i][j]) { M[i][j]=t; dirty=true;}
				}
			}
			vector<double> D1 = M[1];
			double s1=0; for (int i=0; i<l; ++i) s1 += D1[i]; for (int i=0; i<l; ++i) D1[i] /= s1;
			prec.push_back (D1);
		}
		start = now();
	};

	void show    	()      override	{ W.show(); CoarseImage::show(); img.show(); }
	char at      	(coo z) const   	{ return contains(z) && CoarseImage::at(z); }
	void put     	(coo z)         	{ CoarseImage::put(z,1); QT.insert(z); r = std::max (r,sup(z)); }
	bool neighbor	(coo z) const   	{ for (int i=0; i<4; ++i) if (at(z+dz[i])) return true; return false; }

	coo jump (unsigned d) const {
		if (d<=2) return dz[prng.uniform_int(4)];
		if (d<prec.size()) {
			coo w (d-1, prng.discrete(prec[d]) - (d-1));
			if (prng.bernoulli()) w.x = - w.x;
			if (prng.bernoulli()) swap (w.x,w.y);
			return w; }
		if (d<c) return jump(prec.size()-1);
		int l = d - c/2;
		double theta = prng.uniform_real(0,2*M_PI), x = l*cos(theta), y = l*sin(theta);
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

	void paint () override { QT.paint (img,0,512); CoarseImage::paint(); }

	int n; unsigned c; long r;
	Console W;
	QuadTree QT;
	vector<vector<double>> prec;
	Image img;
};

int main (int argc, char ** argv) {
	H.init ("Lattice DLA",argc,argv,"n=2000,p=30,c=50,l=30,f");
	DLA dla (H); dla.show(); dla.runDLA(); dla.output(); if (H['f']) dla.output_fine("dla.png");
	H.L->info ("Final cluster: {} particles, diameter = {}", dla.QT.n, dla.r);
}
