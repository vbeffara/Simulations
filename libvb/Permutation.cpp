#include <vb/Permutation.h>
#include <iostream>

namespace vb {
	Permutation::Permutation (Cycles & c) {
		unsigned sz=0; for (auto v : c) sz += v.size();
		resize(sz);
		for (auto v : c) {
			for (unsigned i=0; i<v.size()-1; ++i) at(v[i])=v[i+1];
			at(v.back()) = v[0];
		}
	}

	Cycles Permutation::cycles () const {
		Cycles c;
		std::vector<unsigned> done (size(), 0);
		for (unsigned i=0; i<size(); ++i) {
			if (done[i]) continue;
			std::vector<unsigned> v (1,i); done[i]=1;
			for (unsigned j=at(i); done[j]==0; j=at(j)) { v.push_back(j); done[j]=1; }
			c.push_back(v);
		}
		return c;
	}

	bool Permutation::is_identity () const {
		for (unsigned i=0; i<size(); ++i) if (at(i)!=i) return false;
		return true;
	}

	Permutation Permutation::inverse () const {
		Permutation s; s.resize(size());
		for (unsigned i=0; i<size(); ++i) s[at(i)]=i;
		return s;
	}

	Permutation Permutation::operator* (const Permutation & o) const {
		Permutation s; s.resize(size());
		for (unsigned i=0; i<size(); ++i) s[i] = o[at(i)];
		return s;
	}

	std::ostream & operator<< (std::ostream &os, const Permutation &P) {
		os << "{"; for (auto i : P) os << " " << i;
		os << " } (";
		for (auto cc : P.cycles()) { os << " ("; for (unsigned i : cc) os << " " << i; os << " )"; }
		return os << " )" << std::endl;
	}
}
