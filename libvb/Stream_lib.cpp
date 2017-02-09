#include <vb/Stream_lib.h>

namespace vb {
	Stream <std::vector<unsigned long>> partitions (unsigned long n, unsigned long m) {
		return Stream <std::vector<unsigned long>> ([n,m](Sink <std::vector<unsigned long>> & yield) {
			for (unsigned long i=m; i<n; ++i) {
				for (auto p : partitions(n-i,i)) {
					std::vector<unsigned long> out ({i});
					for (auto j : p) out.push_back(j);
					yield (out);
				}
			}
			if (n>=m) yield ({n});
		});
	}

	Stream <std::vector<unsigned long>> tuples (unsigned long k, unsigned long n) {
		return Stream <std::vector<unsigned long>> ([k,n](Sink<std::vector<unsigned long>> & yield) {
			if (k==0) { yield ({}); return; }
			for (unsigned long i=0; i<n; ++i) {
				for (auto c : tuples (k-1,n-1)) {
					std::vector<unsigned long> out ({i});
					for (auto j : c) out.push_back (j<i ? j : j+1);
					yield (out);
				}
			}
		});
	}

	Stream <std::vector<unsigned long>> cycles (unsigned long k, unsigned long n) {
		return Stream <std::vector<unsigned long>> ([k,n](Sink <std::vector<unsigned long>> & yield) {
			for (unsigned long i=0; i<n-k+1; ++i) {
				for (auto c : tuples (k-1,n-i-1)) {
					std::vector<unsigned long> out (1,i);
					for (auto j : c) out.push_back (i+j+1);
					yield (out);
				}
			}
		});
	}

	Stream <Permutation> permutations (unsigned long n) {
		return Stream<Permutation> ([n](Sink<Permutation> & yield) {
			Permutation p (n);
			do yield(Permutation(p)); while (next_permutation(p.begin(),p.end()));
		});
	}

	Stream <Permutation> permutations (std::vector<unsigned long> s) {
		return Stream <Permutation> ([s](Sink <Permutation> & yield) {
			int n=0; for (auto i : s) n += i; if (n==0) { yield ({{}}); return; }
			unsigned L=0; for (unsigned i=0; i<s.size(); ++i) if (s[i]>L) {
				L=s[i]; std::vector<unsigned long> ns = s; ns[i]=0;
				for (auto c : tuples (L-1,n-1)) {
					std::vector<unsigned long> cc ({0});
					for (auto i : c) cc.push_back(i+1);
					std::vector<unsigned long> missed (n);
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

	Stream <Hypermap> hypermaps (const std::vector<unsigned long> & s, const std::vector<unsigned long> & a, const std::vector<unsigned long> & p) {
		Cycles cs; int i=0;
		for (int l : s) { std::vector<unsigned long> c; for (int j=0; j<l; ++j) c.push_back(i++); cs.push_back (c); }
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
}