#include <vb/Pairings.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>
#include <vb/Toroidal.h>

using namespace vb; using namespace std;

vector<unsigned> ntri { 0, 1, 2, 6, 23, 92/*, 467*/ }; // 92 is probably ok, the others ...

int main (int argc, char ** argv) {
	Hub H ("Toroidal enumeration", argc, argv, "s=1,m=4,r=1");
	int s=H['s'], a=6*s, r=H['r']; if (r>0) prng.seed(r);

	Cycles phi_c; for (unsigned i=0; i<a/3; ++i) phi_c.push_back ({3*i,3*i+1,3*i+2}); Permutation phi(phi_c);

	vector<Hypermap> v; unsigned target = s<ntri.size() ? ntri[s] : 1000;

	while (v.size()<target) {
		Permutation alpha = Pairings(a).rand();                                     	if (!connected(phi,alpha))	continue;
		Permutation sigma = (alpha*phi).inverse(); Hypermap M (sigma,alpha,phi);    	if (M.genus() != 1)       	continue;
		bool good=true; for (auto & c : sigma.cycles()) if (c.size()<=3) good=false;	if (!good)                	continue;

		M.normalize();
		bool there = false; for (Hypermap & O : v) if (O==M) there = true;
		if (!there) {
			v.push_back(M);
			cout << "Sigma: " << M.sigma << endl;
			cout << "Alpha: " << M.alpha << endl;
			cout << "Phi:   " << M.phi << endl;

			ostringstream os; os << "Toroidal enumeration (s=" << s << ", pass " << M.sigma.passport() << ", i=" << v.size() << ")"; H.title = os.str();
			Toroidal T (M,H); for (int i=0; i<3; ++i) { T.split_edges(); } T.pack(1);

			cerr << "               \r\n";
			cout << " --> Passport: " << M.sigma.passport() << endl;
			cout << "     Modulus: tau=" << T.m << endl;
			cout << "     Order number: " << v.size() << endl;
			cout << endl;
			T.output_pdf();
		}
	}
}