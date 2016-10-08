#include <vb/Stream.h>

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
}
