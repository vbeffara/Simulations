#include <vb/Permutation.h>
#include <algorithm>
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

	std::vector<unsigned> Permutation::signature () const {
		vector<unsigned> output;
		for (auto c : cycles()) output.push_back(c.size());
		sort (output.begin(),output.end());
		return output;
	}

	Passport Permutation::passport () const {
		std::vector<unsigned> s = signature();
		Passport out;
		unsigned l=0, c=0;
		for (int i=s.size()-1; i>=0; --i) {
			if (s[i]==l) ++c;
			else {
				if (c>0) out.push_back({l,c});
				l=s[i]; c=1;
			}
		}
		out.push_back({l,c});
		return out;
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

	Permutation Permutation::conjugate (const Permutation & s) const {
		Permutation out(size());
		for (int i=0; i<size(); ++i) out[s[i]]=s[at(i)];
		return out;
	}

	std::ostream & operator<< (std::ostream &os, const Permutation &P) {
		os << "{"; for (auto i : P) os << " " << i;
		os << " } (";
		for (auto cc : P.cycles()) { os << " ("; for (unsigned i : cc) os << " " << i; os << " )"; }
		return os << " )" << std::endl;
	}

	std::ostream & operator<< (std::ostream &os, const Passport &P) {
		bool first = true;
		for (auto c : P) { if (!first) os << " "; os << c.first << "(" << c.second << ")"; first = false; }
		return os;
	}
}
