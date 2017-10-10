#include <vb/Stream_lib.h>

namespace vb {
	Stream <std::vector<int>> partitions (int n, int m) {
		return Stream <std::vector<int>> ([n,m](Sink <std::vector<int>> & yield) {
			for (int i=m; i<n; ++i) {
				for (auto p : partitions(n-i,i)) {
					std::vector<int> out ({i});
					for (auto j : p) out.push_back(j);
					yield (out);
				}
			}
			if (n>=m) yield ({n});
		});
	}

	Stream <std::vector<int>> tuples (int k, int n) {
		return Stream <std::vector<int>> ([k,n](Sink<std::vector<int>> & yield) {
			if (k==0) { yield ({}); return; }
			for (int i=0; i<n; ++i) {
				for (auto c : tuples (k-1,n-1)) {
					std::vector<int> out ({i});
					for (auto j : c) out.push_back (j<i ? j : j+1);
					yield (out);
				}
			}
		});
	}

	Stream <std::vector<int>> cycles (int k, int n) {
		return Stream <std::vector<int>> ([k,n](Sink <std::vector<int>> & yield) {
			for (int i=0; i<n-k+1; ++i) {
				for (auto c : tuples (k-1,n-i-1)) {
					std::vector<int> out (1,i);
					for (auto j : c) out.push_back (i+j+1);
					yield (out);
				}
			}
		});
	}

	Stream <Permutation> permutations (int n) {
		return Stream<Permutation> ([n](Sink<Permutation> & yield) {
			Permutation p (n);
			do yield(Permutation(p)); while (next_permutation(p.begin(),p.end()));
		});
	}

	Stream <Permutation> permutations (std::vector<int> s) {
		return Stream <Permutation> ([s](Sink <Permutation> & yield) {
			int n=0; for (auto i : s) n += i; if (n==0) { yield ({{}}); return; }
			int L=0; for (int i=0; i<s.size(); ++i) if (s[i]>L) {
				L=s[i]; std::vector<int> ns = s; ns[i]=0;
				for (auto c : tuples (L-1,n-1)) {
					std::vector<int> cc ({0});
					for (auto i : c) cc.push_back(i+1);
					std::vector<int> missed (n);
					for (int i=0; i<n; ++i) missed[i]=i;
					for (auto i : cc) missed[i]=0;
					for (int i=0, j=0; j<n; ++j) if (missed[j]) missed[i++] = missed[j];
					for (auto & p : permutations(ns)) {
						auto out = p.cycles();
						for (auto & c : out) for (auto & i : c) i = missed[i];
						out.push_back (cc); yield (out);
					}
				}
			}
		});
	}

	Stream <Hypermap> hypermaps (const std::vector<int> & s, const std::vector<int> & a, const std::vector<int> & p) {
		Cycles cs; int i=0;
		for (int l : s) {
			std::vector<int> c; c.reserve(l);
			for (int j=0; j<l; ++j) c.push_back(i++);
			cs.push_back (c);
		}
		Permutation sigma (cs);
		return Stream<Hypermap> ([sigma,a,p](Sink<Hypermap> & yield) {
			std::vector<Hypermap> hs;
			for (const auto & alpha : permutations(a)) {
				if (!connected(sigma,alpha)) continue;
				Permutation phi = (sigma*alpha).inverse(); if (phi.signature() != p) continue;
				Hypermap h(sigma,alpha,phi);
				h.normalize(); bool done=false;
				for (auto & hh : hs) if (h==hh) { done=true; break; }
				if (!done) { hs.push_back(h); yield(h); }
			}
		});
	}
} // namespace vb
