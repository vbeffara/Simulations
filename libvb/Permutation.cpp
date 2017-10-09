#include <vb/Permutation.h>
#include <algorithm>

namespace vb {
	Permutation::Permutation (Cycles & c) {
		int sz=0; for (const auto & v : c) sz += v.size();
		resize(sz);
		for (auto v : c) {
			for (int i=0; i<v.size()-1; ++i) at(v[i])=v[i+1];
			at(v.back()) = v[0];
		}
	}

	Cycles Permutation::cycles () const {
		Cycles c;
		std::vector<int> done (size(), 0);
		for (int i=0; i<size(); ++i) {
			if (done[i]) continue;
			std::vector<int> v (1,i); done[i]=1;
			for (int j=at(i); done[j]==0; j=at(j)) { v.push_back(j); done[j]=1; }
			c.push_back(v);
		}
		return c;
	}

	std::vector<int> Permutation::signature () const {
		vector<int> output;
		for (const auto & c : cycles()) output.push_back(c.size());
		sort (output.begin(),output.end());
		return output;
	}

	Passport Permutation::passport () const {
		std::vector<int> s = signature();
		Passport out;
		int l=0, c=0;
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
		for (int i=0; i<size(); ++i) if (at(i)!=i) return false;
		return true;
	}

	Permutation Permutation::inverse () const {
		Permutation s; s.resize(size());
		for (int i=0; i<size(); ++i) s[at(i)]=i;
		return s;
	}

	Permutation Permutation::operator* (const Permutation & o) const {
		Permutation s; s.resize(size());
		for (int i=0; i<size(); ++i) s[i] = o[at(i)];
		return s;
	}

	Permutation Permutation::conjugate (const Permutation & s) const {
		Permutation out(size());
		for (int i=0; i<size(); ++i) out[s[i]]=s[at(i)];
		return out;
	}

	Permutation Transposition (int n, int i, int j) {
		Permutation p(n); p[i]=j; p[j]=i; return p;
	}

	bool connected (const Permutation & s, const Permutation & a) {
		int n=s.size(); std::vector<int> l(n); for (int i=0; i<n; ++i) l[i]=i;
		bool dirty=true; while (dirty) {
			dirty=false;
			for (int i=0; i<n; ++i) if (l[s[i]]<l[i]) { l[i]=l[s[i]]; dirty=true; }
			for (int i=0; i<n; ++i) if (l[a[i]]<l[i]) { l[i]=l[a[i]]; dirty=true; }
		}
		for (int i=0; i<n; ++i) if (l[i]>0) return false;
		return true;
	}

	std::ostream & operator<< (std::ostream &os, const Permutation &P) {
		os << "("; bool f = true;
		for (auto cc : P.cycles()) {
			if (!f) os << " ";
			os << "("; bool ff = true;
			for (int i : cc) { if (!ff) os << " "; os << i; ff = false; }
			os << ")"; f = false;
		}
		return os << ")";
	}

	std::ostream & operator<< (std::ostream &os, const Passport &P) {
		bool first = true;
		for (auto c : P) { if (!first) os << " "; os << c.first << "(" << c.second << ")"; first = false; }
		return os;
	}
}
