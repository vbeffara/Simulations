#include <vb/Hub.h>
#include <vb/Hypermap.h>
#include <vb/Stream.h>
#include <algorithm>

using namespace vb; using namespace std;

Stream <vector<unsigned>> tuples (unsigned k, unsigned n) {
	return Stream <vector<unsigned>> ([k,n](Sink<vector<unsigned>> & yield) {
		if (k==0) { yield ({}); return; }
		for (unsigned i=0; i<n; ++i) {
			for (auto c : tuples (k-1,n-1)) {
				vector<unsigned> out ({i});
				for (auto j : c) out.push_back (j<i ? j : j+1);
				yield (out);
			}
		}
	});
}

Stream <vector<unsigned>> cycles (int k, int n) {
	return Stream <vector<unsigned>> ([k,n](Sink <vector<unsigned>> & yield) {
		for (int i=0; i<n-1; ++i) {
			for (auto c : tuples (k-1,n-i-1)) {
				vector<unsigned> out ({unsigned(i)});
				for (auto j : c) out.push_back (i+j+1);
				yield (out);
			}
		}
	});
}

auto permutations (int n) {
	return Stream<Permutation> ([n](Sink<Permutation> & yield) {
		Permutation p (n);
		do yield(p); while (next_permutation(p.begin(),p.end()));
	});
}

auto permutations (vector<unsigned> s) {
	return Stream<Permutation> ([s](Sink<Permutation> & yield) {
		int n=0; for (auto i : s) n += i;
		for (auto p : permutations(n)) if (p.signature() == s) yield(p);
	});
}

auto maps (vector<unsigned> s, vector<unsigned> a, vector<unsigned> p) {
	Cycles cs; int i=0;
	for (int l : s) { vector<unsigned> c; for (int j=0; j<l; ++j) c.push_back(i++); cs.push_back (c); }
	Permutation sigma (cs);
	return Stream<Hypermap> ([sigma,a,p](Sink<Hypermap> & yield) {
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

int main (int argc, char ** argv) {
	H.init ("Streams and permutations", argc, argv, "k=2,n=4");
	for (auto x : cycles (H['k'],H['n'])) { for (auto i : x) cout << i; cout << endl; }
	// int n=0;
	// for (auto m : maps ({2,2,3,3}, {2,2,2,2,2}, {3,3,4})) { cout << m; ++n; }
	// cout << "Total: " << n << " hypermap(s)" << endl;
}
