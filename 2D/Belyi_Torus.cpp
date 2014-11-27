// d: minimum degree of the triangulation
// f: identify maps obtained after mirror flip
// g: genus of the triangulation
// m: mode for the generated pdf (see vb/Toroidal.h)
// n: number of subdivisions for the circle packing
// o: enable splitting, circle packing and output (fails if g!=1)
// r: seed for the PRNG, set to 0 for time-based
// s: number of vertices

#include <vb/Constellation1.h>
#include <vb/Pairings.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>

using namespace vb; using namespace std;

vector<unsigned> ntri { 0, 1, 5, 46, 669 };

int main (int argc, char ** argv) {
	Hub H ("Toroidal enumeration", argc, argv, "s=1,m=228,r=1,o,d=0,D=0,g=1,f,n=2");
	unsigned s=H['s'], g=H['g'], a=6*(s+2*g-2), r=H['r'], d=H['d'], D=H['D'];
	assert (a>0); if (g!=1) assert(!H['o']); if (r>0) prng.seed(r);

	Cycles phi_c; for (unsigned i=0; i<a/3; ++i) phi_c.push_back ({3*i,3*i+1,3*i+2}); Permutation phi(phi_c);

	vector<Hypermap> v; unsigned target = 0; if ((d==0) && (g==1) && (!H['f']) && (s<ntri.size())) target = ntri[s];

	Image img (500,500,"Belyi_Torus");
	while ((target==0)||(v.size()<target)) {
		Permutation alpha = Pairings(a).rand();                                        	if (!connected(phi,alpha))	continue;
		Permutation sigma = (alpha*phi).inverse(); Hypermap M (sigma,alpha,phi);       	if (M.genus() != g)       	continue;
		bool good=true; for (auto & c : sigma.cycles()) if (c.size()<d) good=false;    	if (!good)                	continue;
		good=true; if (D>0) for (auto & c : sigma.cycles()) if (c.size()>D) good=false;	if (!good)                	continue;

		M.normalize();
		bool there = false; for (Hypermap & O : v) if (O==M) there = true;
		if (H['f']) { Hypermap MM = M; MM.mirror(); MM.normalize(); for (Hypermap & O : v) if (O==MM) there = true; }
		if (!there) {
			v.push_back(M);
			cout << "Sigma: " << M.sigma << endl;
			cout << "Alpha: " << M.alpha << endl;
			cout << "Phi:   " << M.phi << endl;
			cout << endl;
			cout << "     Order number:    " << v.size() << endl;
			cout << "     Passport:        " << M.sigma.passport() << endl;

			ostringstream os; os << "Toroidal enumeration (s=" << s << ", pass " << M.sigma.passport() << ", i=" << v.size() << ")"; H.title = os.str();
			Constellation1<double> C {M,H,H['n']}; double er = C.findn();
			if (er>.01) { double u = .1; while ((er>.01)&&(u>1e-20)) { C.find(u); er=C.findn(); u/=10; } }
			cout << "     Final error:     " << er << endl;
			cout << "     Modulus:         " << C.tau << endl;
			cout << "     Klein invariant: " << j_(q_(C.tau)) << endl;
			cout << endl;

			if (H['o']) { img.title = H.title; img.label(img.title.c_str()); if (!img.visible()) img.show(); C.draw(img); img.output(); }
		}
	}
}
