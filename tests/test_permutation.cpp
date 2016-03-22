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
		for (int i=0; i<n-k+1; ++i) {
			for (auto c : tuples (k-1,n-i-1)) {
				vector<unsigned> out ({unsigned(i)});
				for (auto j : c) out.push_back (i+j+1);
				yield (out);
			}
		}
	});
}

Stream <Permutation> by_cycles (vector<unsigned> s) {
	return Stream <Permutation> ([s](Sink <Permutation> & yield) {
		int n=0; for (auto i : s) n += i; if (n==0) { yield ({{}}); return; }
		unsigned L=0; for (int i=0; i<s.size(); ++i) if (s[i]>L) {
			L=s[i]; vector<unsigned> ns = s; ns[i]=0;
			for (auto c : tuples (L-1,n-1)) {
				vector<unsigned> cc ({0});
				for (auto i : c) cc.push_back(i+1);
				vector<unsigned> missed (n);
				for (int i=0; i<n; ++i) missed[i]=i; for (auto i : cc) missed[i]=0;
				for (int i=0, j=0; j<n; ++j) if (missed[j]) missed[i++] = missed[j];
				for (auto p : by_cycles(ns)) {
					auto out = p.cycles();
					for (auto & c : out) for (auto & i : c) i = missed[i];
					out.push_back (cc); yield (out);
				}
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
	for (auto x : by_cycles ({2,2,2})) { cout << x << endl; }
	// int n=0;
	// for (auto m : maps ({2,2,3,3}, {2,2,2,2,2}, {3,3,4})) { cout << m; ++n; }
	// cout << "Total: " << n << " hypermap(s)" << endl;
}
