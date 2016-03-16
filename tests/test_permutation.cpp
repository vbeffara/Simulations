#include <vb/Hypermap.h>
#include <algorithm>

using namespace vb;
using namespace std;

vector<Hypermap> maps (vector<unsigned> s, vector<unsigned> a, vector<unsigned> p) {
	Cycles cs; int i=0;
	for (int l : s) { vector<unsigned> c; for (int j=0; j<l; ++j) c.push_back(i++); cs.push_back (c); }
	Permutation sigma (cs), alpha (sigma.size());
	vector<Hypermap> output;
	do {
		if (alpha.signature() != a) continue;
		Permutation phi = (sigma*alpha).inverse(); if (phi.signature() != p) continue;
		output.push_back (Hypermap(sigma,alpha,phi)); cerr<<".";
	} while (next_permutation(alpha.begin(),alpha.end()));
	return output;
}

int main (int, char **) {
	auto ms = maps ({2,2,3}, {2,2,3}, {2,2,3});
	for (auto m : ms) {
		cout << m;
		cout << "  sigma:     " << m.sigma;
		cout << "  alpha:     " << m.alpha;
		cout << "  phi:       " << m.phi;
		cout << "  connected: " << connected (m.sigma,m.alpha) << endl;
	}
	cout << "Total: " << ms.size() << " hypermap(s)" << endl;
}
