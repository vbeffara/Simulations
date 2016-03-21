#include <vb/Hypermap.h>
#include <vb/Stream.h>
#include <algorithm>

using namespace vb; using namespace std;

auto maps (vector<unsigned> s, vector<unsigned> a, vector<unsigned> p) {
	return Stream<Hypermap> ([s,a,p](Sink<Hypermap> & yield) {
		Cycles cs; int i=0;
		for (int l : s) { vector<unsigned> c; for (int j=0; j<l; ++j) c.push_back(i++); cs.push_back (c); }
		Permutation sigma (cs), alpha (sigma.size());
		vector<Hypermap> hs;
		do {
			if (alpha.signature() != a) continue;
			if (!connected(sigma,alpha)) continue;
			Permutation phi = (sigma*alpha).inverse(); if (phi.signature() != p) continue;
			Hypermap h(sigma,alpha,phi);
			h.normalize(); bool done=0;
			for (auto & hh : hs) if (h==hh) { done=1; break; }
			if (!done) { hs.push_back(h); yield(h); }
		} while (next_permutation(alpha.begin(),alpha.end()));
	});
}

int main (int, char **) {
	int n=0;
	for (auto m : maps (vector<unsigned> {2,2,3,3}, vector<unsigned> {2,2,2,2,2}, vector<unsigned> {3,3,4})) {
		cout << m;
		++n;
	}
	cout << "Total: " << n << " hypermap(s)" << endl;
}
