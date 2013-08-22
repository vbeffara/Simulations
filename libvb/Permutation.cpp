#include <vb/Permutation.h>

namespace vb {
	void Permutation::use_s () {
		c.clear();
		std::vector<int> done (size(), 0);
		for (int i=0; i<size(); ++i) {
			if (done[i]) continue;
			std::vector<int> v (1,i); done[i]=1;
			for (int j=at(i); done[j]==0; j=at(j)) { v.push_back(j); done[j]=1; }
			c.push_back(v);
		}
		labels();
	}

	void Permutation::use_c () {
		int sz=0; for (std::vector<int> v : c) sz += v.size();
		std::vector<int>::operator= (std::vector<int> (sz));
		for (auto v : c) {
			for (int i=0; i<v.size()-1; ++i) at(v[i])=v[i+1];
			at(v.back()) = v[0];
		}
		labels();
	}

	std::ostream & operator<< (std::ostream &os, Permutation &P) {
		os << "(";
		for (auto cc : P.c) { os << " ("; for (int i : cc) os << " " << i; os << " )"; }
		return os << " )" << std::endl;
	}
}
