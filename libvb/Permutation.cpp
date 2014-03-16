#include <vb/Permutation.h>
#include <iostream>

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
	}

	void Permutation::use_c () {
		int sz=0; for (std::vector<int> v : c) sz += v.size();
		std::vector<int>::operator= (std::vector<int> (sz));
		for (auto v : c) {
			for (int i=0; i<v.size()-1; ++i) at(v[i])=v[i+1];
			at(v.back()) = v[0];
		}
	}

	bool Permutation::is_identity () {
		for (int i=0; i<size(); ++i) if (at(i)!=i) return false;
		return true;
	}

	Permutation Permutation::inverse () const {
		Permutation s; s.resize(size());
		for (int i=0; i<size(); ++i) s[at(i)]=i;
		s.use_s(); return s;
	}

	Permutation Permutation::operator* (const Permutation & o) const {
		Permutation s; s.resize(size());
		for (int i=0; i<size(); ++i) s[i] = o[at(i)];
		s.use_s(); return s;
	}

	std::ostream & operator<< (std::ostream &os, Permutation &P) {
		os << "(";
		for (auto cc : P.cycles()) { os << " ("; for (int i : cc) os << " " << i; os << " )"; }
		return os << " )" << std::endl;
	}
}
