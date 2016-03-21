#include <vb/Hypermap.h>
#include <vb/Stream.h>
#include <algorithm>

using namespace vb; using namespace std;

auto permutations (vector<unsigned> s) {
	return Stream<Permutation> ([s](Sink<Permutation> & yield) {
		int n=0; for (auto i : s) n += i; Permutation p (n);
		do { if (p.signature() == s) yield(p); } while (next_permutation(p.begin(),p.end()));
	});
}

auto maps (vector<unsigned> s, vector<unsigned> a, vector<unsigned> p) {
	return Stream<Hypermap> ([s,a,p](Sink<Hypermap> & yield) {
		Cycles cs; int i=0;
		for (int l : s) { vector<unsigned> c; for (int j=0; j<l; ++j) c.push_back(i++); cs.push_back (c); }
		Permutation sigma (cs);
		vector<Hypermap> hs;
		for (auto alpha : permutations(a)) {
			if (!connected(sigma,alpha)) continue;
			Permutation phi = (sigma*alpha).inverse(); if (phi.signature() != p) continue;
			Hypermap h(sigma,alpha,phi);
			h.normalize(); bool done=0;
			for (auto & hh : hs) if (h==hh) { done=1; break; }
			if (!done) { hs.push_back(h); yield(h); }
		}
	});
}

int main (int, char **) {
	int n=0;
	for (auto m : maps ({2,2,3,3}, {2,2,2,2,2}, {3,3,4})) {
		cout << m;
		++n;
	}
	cout << "Total: " << n << " hypermap(s)" << endl;
}
