// d: minimum degree of the triangulation
// g: genus of the triangulation
// m: mode for the generated pdf (see vb/Toroidal.h)
// o: enable splitting, circle packing and output (fails if g!=1)
// r: seed for the PRNG, set to 0 for time-based
// s: number of vertices

#include <vb/Pairings.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>
#include <vb/Toroidal.h>

using namespace vb; using namespace std;

vector<unsigned> ntri { 0, /*1, 3, 11, 64, 455 */ };

int main (int argc, char ** argv) {
	Hub H ("Toroidal enumeration", argc, argv, "s=1,m=4,r=1,o,d=0,g=1");
	int s=H['s'], g=H['g'], a=6*(s+2*g-2), r=H['r'], d=H['d'];
	assert (a>0); if (g!=1) assert(!H['o']); if (r>0) prng.seed(r);

	Cycles phi_c; for (unsigned i=0; i<a/3; ++i) phi_c.push_back ({3*i,3*i+1,3*i+2}); Permutation phi(phi_c);

	vector<Hypermap> v; unsigned target = s<ntri.size() ? ntri[s] : 0;

	while ((target==0)||(v.size()<target)) {
		Permutation alpha = Pairings(a).rand();                                    	if (!connected(phi,alpha))	continue;
		Permutation sigma = (alpha*phi).inverse(); Hypermap M (sigma,alpha,phi);   	if (M.genus() != g)       	continue;
		bool good=true; for (auto & c : sigma.cycles()) if (c.size()<d) good=false;	if (!good)                	continue;

		M.normalize();
		bool there = false; for (Hypermap & O : v) if (O==M) there = true;
		if (!there) {
			v.push_back(M);

			cout << "Sigma: " << M.sigma << endl;
			cout << "Alpha: " << M.alpha << endl;
			cout << "Phi:   " << M.phi << endl;

			cerr << "               \r\n";
			cout << " --> Passport: " << M.sigma.passport() << endl;
			cout << "     Order number: " << v.size() << endl << endl;

			if (H['o']) {
				ostringstream os; os << "Toroidal enumeration (s=" << s << ", pass " << M.sigma.passport() << ", i=" << v.size() << ")"; H.title = os.str();
				Toroidal T (M,H); for (int i=0; i<3; ++i) { T.split_edges(); } T.pack(1);
				cout << "     Modulus: tau=" << T.m << endl << endl;
				T.output_pdf();
			}
		}
	}
}