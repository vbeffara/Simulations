#include <vb/Permutation.h>
#include <algorithm>

namespace vb {
	Permutation::Permutation (Cycles & c) {
		unsigned sz=0; for (const auto & v : c) sz += v.size();
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
		for (const auto & c : cycles()) output.push_back(c.size());
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
		for (unsigned i=0; i<size(); ++i) out[s[i]]=s[at(i)];
		return out;
	}

	Permutation Transposition (unsigned n, unsigned i, unsigned j) {
		Permutation p(n); p[i]=j; p[j]=i; return p;
	}

	bool connected (const Permutation & s, const Permutation & a) {
		unsigned n=s.size(); std::vector<unsigned> l(n); for (unsigned i=0; i<n; ++i) l[i]=i;
		bool dirty=true; while (dirty) {
			dirty=false;
			for (unsigned i=0; i<n; ++i) if (l[s[i]]<l[i]) { l[i]=l[s[i]]; dirty=true; }
			for (unsigned i=0; i<n; ++i) if (l[a[i]]<l[i]) { l[i]=l[a[i]]; dirty=true; }
		}
		for (unsigned i=0; i<n; ++i) if (l[i]>0) return false;
		return true;
	}

	Stream <Permutation> permutations (int n) {
		return Stream<Permutation> ([n](Sink<Permutation> & yield) {
			Permutation p (n);
			do yield(p); while (next_permutation(p.begin(),p.end()));
		});
	}

	Stream <Permutation> permutations (std::vector<unsigned> s) {
		return Stream <Permutation> ([s](Sink <Permutation> & yield) {
			int n=0; for (auto i : s) n += i; if (n==0) { yield ({{}}); return; }
			unsigned L=0; for (unsigned i=0; i<s.size(); ++i) if (s[i]>L) {
				L=s[i]; std::vector<unsigned> ns = s; ns[i]=0;
				for (auto c : tuples (L-1,n-1)) {
					std::vector<unsigned> cc ({0});
					for (auto i : c) cc.push_back(i+1);
					std::vector<unsigned> missed (n);
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

	std::ostream & operator<< (std::ostream &os, const Permutation &P) {
		os << "("; bool f = true;
		for (auto cc : P.cycles()) {
			if (!f) os << " ";
			os << "("; bool ff = true;
			for (unsigned i : cc) { if (!ff) os << " "; os << i; ff = false; }
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
