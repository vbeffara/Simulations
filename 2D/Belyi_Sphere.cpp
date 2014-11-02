// c: do not draw face centers
// d: minimum degree of the triangulation
// f: identify maps obtained after mirror flip
// g: genus of the triangulation
// m: mode for the generated pdf (see vb/Toroidal.h)
// n: number of subdivisions for the circle packing
// o: enable splitting, circle packing and output (fails if g!=1)
// r: seed for the PRNG, set to 0 for time-based
// s: number of vertices

// S-E+F = 2, 3F=2E, S=2+F/2, F=2d, S=2+d

#include <vb/Pairings.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>
#include <vb/Constellation.h>

using namespace vb; using namespace std;

vector<unsigned> ntri { 0, 1, 5, 46, 669 };

int main (int argc, char ** argv) {
	Hub H ("Spheroidal enumeration", argc, argv, "s=3,m=228,r=1,o,d=0,D=0,g=0,f,n=2,c");
	unsigned s=H['s'], g=0, a=6*(s-2), r=H['r'], d=H['d'], D=H['D'];
	assert (a>0); if (r>0) prng.seed(r);

	Cycles phi_c; for (unsigned i=0; i<a/3; ++i) phi_c.push_back ({3*i,3*i+1,3*i+2}); Permutation phi(phi_c);

	vector<Hypermap> v; unsigned target = 0; if ((d==0) && (g==1) && (!H['f']) && (s<ntri.size())) target = ntri[s];

	while ((target==0)||(v.size()<target)) {
		Permutation alpha = Pairings(a).rand();                                    	if (!connected(phi,alpha))	continue;
		Permutation sigma = (alpha*phi).inverse(); Hypermap M (sigma,alpha,phi);   	if (M.genus() != g)       	continue;
		bool good=true; for (auto & c : sigma.cycles()) if (c.size()<d) good=false;	if (!good)                	continue;
		if (D>0) for (auto & c : sigma.cycles()) if (c.size()>D) good=false;       	if (!good)                	continue;
		if (H['o']) for (auto & c : sigma.cycles()) if (c.size()%2) good=false;    	if (!good)                	continue;

		M.normalize();
		bool there = false; for (Hypermap & O : v) if (O==M) there = true;
		if (H['f']) { Hypermap MM = M; MM.mirror(); MM.normalize(); for (Hypermap & O : v) if (O==MM) there = true; }
		if (!there) {
			v.push_back(M);

			cout << "Sigma: " << M.sigma << endl;
			cout << "Alpha: " << M.alpha << endl;
			cout << "Phi:   " << M.phi << endl;
			cout << endl;
			cout << "     Order number: " << v.size() << endl;
			cout << "     Name:         " << M.name() << endl;
			cout << "     Passport:     " << M.sigma.passport() << endl;
			cout << endl;

			if (H['o']) {
				ostringstream os; os << "Spheroidal enumeration (s=" << s << ", pass " << M.sigma.passport() << ", i=" << v.size() << ")"; H.title = os.str();
				// int nsub = H['n']; if (!H['c']) { M.dessin(); --nsub; } for (int i=0; i<nsub; ++i) M.split_edges(); M.simplify(1);
				// Toroidal T (M,H); T.pack();
				// cout << "     Modulus: tau=" << T.m << endl << endl;
				Constellation<double> C {M,H,H['n'],false}; C.show(); C.findm(); C.belyi(); cout << endl << C << endl; C.img->hide();
				// T.output_pdf();
			}
		}
	}
}
