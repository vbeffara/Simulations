#include <vb/Stream.h>

namespace vb {
	Stream <std::vector<unsigned>> partitions (unsigned n, unsigned m) {
		return Stream <std::vector<unsigned>> ([n,m](Sink <std::vector<unsigned>> & yield) {
			for (unsigned i=m; i<n; ++i) {
				for (auto p : partitions(n-i,i)) {
					std::vector<unsigned> out ({i});
					for (auto j : p) out.push_back(j);
					yield (out);
				}
			}
			if (n>=m) yield ({n});
		});
	}

	Stream <std::vector<unsigned>> tuples (unsigned k, unsigned n) {
		return Stream <std::vector<unsigned>> ([k,n](Sink<std::vector<unsigned>> & yield) {
			if (k==0) { yield ({}); return; }
			for (unsigned i=0; i<n; ++i) {
				for (auto c : tuples (k-1,n-1)) {
					std::vector<unsigned> out ({i});
					for (auto j : c) out.push_back (j<i ? j : j+1);
					yield (out);
				}
			}
		});
	}

	Stream <std::vector<unsigned>> cycles (int k, int n) {
		return Stream <std::vector<unsigned>> ([k,n](Sink <std::vector<unsigned>> & yield) {
			for (int i=0; i<n-k+1; ++i) {
				for (auto c : tuples (k-1,n-i-1)) {
					std::vector<unsigned> out ({unsigned(i)});
					for (auto j : c) out.push_back (i+j+1);
					yield (out);
				}
			}
		});
	}
}
