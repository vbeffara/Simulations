#include <vb/Toroidal.h>
#include <cassert>

// S-A+F=0. A = (3/2)F. S=A-F=F/2. a=3F.
// sigma alpha phi = id

using namespace vb; using namespace std;

bool connected (Permutation s, Permutation a) {
	int n=s.size(); vector<int> l(n); for (int i=0; i<n; ++i) l[i]=i;
	bool dirty=true; while (dirty) {
		dirty=false;
		for (int i=0; i<n; ++i) if (l[s[i]]<l[i]) { l[i]=l[s[i]]; dirty=true; }
		for (int i=0; i<n; ++i) if (l[a[i]]<l[i]) { l[i]=l[a[i]]; dirty=true; }
	}
	for (int i=0; i<n; ++i) if (l[i]>0) return false;
	return true;
}

vector<Cycles> pairings_go (vector<unsigned> & p) {
	if (p.size() == 0) return {};
	if (p.size() == 2) return {{p}};
	vector<Cycles> out; int i=p[0];
	for (unsigned o : p) {
		if ((o/3)==(i/3)) continue;
		vector<unsigned> new_p;
		for (unsigned j : p) if ((j!=i) && (j!=o)) new_p.push_back(j);
		for (auto & c : pairings_go (new_p)) {
			c.push_back ({p[0],o});
			out.push_back (c);
		}
	}
	return out;
}

vector<Permutation> pairings (int a) {
	vector<unsigned> all; for (int i=0; i<a; ++i) all.push_back(i);
	vector<Permutation> out;
	for (auto & c : pairings_go(all)) out.push_back (c);
	return out;
}

int main (int argc, char ** argv) {
	Hub H ("Toroidal enumeration", argc, argv, "f=4,m=17");
	int F=H['f'], A=3*F/2, S=F/2, a=3*F; assert (F%2 == 0);

	Cycles phi_c; for (unsigned i=0; i<a/3; ++i) phi_c.push_back ({3*i,3*i+1,3*i+2}); Permutation phi(phi_c);

	vector<Permutation> alphas = pairings(a);
	cout << alphas.size() << " pairings." << endl << endl;

	int i=0; for (Permutation & alpha : alphas) {
		                                                                            	if (!connected(phi,alpha))	continue;
		Permutation sigma = (alpha*phi).inverse(); Hypermap M (sigma,alpha,phi);    	if (M.genus() != 1)       	continue;
		bool good=true; for (auto & c : sigma.cycles()) if (c.size()<=2) good=false;	if (!good)                	continue;

		// cout << "Sigma: " << sigma;
		// cout << "Alpha: " << alpha;
		// cout << "Phi:   " << phi;

		// ostringstream os; os << "Toroidal enumeration (f=" << F << ", i=" << i << ")"; H.title = os.str();
		// Toroidal T (M,H);
		// T.split_edges(); T.acpa(2.7);
		// T.split_edges(); T.acpa(2.7);
		// T.split_edges(); T.pack();

		// cout << "  " << T.m << endl;
		// cout << H.title << endl;
		// T.output_pdf();

		// cout << endl;
		++i;
	}

	cout << i << " triangulations." << endl;
}