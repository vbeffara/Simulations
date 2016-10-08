#include <vb/Pairings.h>
#include <vb/PRNG.h>
#include <vb/math.h>

namespace vb {
	void Pairings_Iterator::next () {
		while (todo_c.size()>0) {
			std::vector<unsigned> c = todo_c.back(); todo_c.pop_back();
			std::vector<unsigned> p = todo_p.back(); todo_p.pop_back();
			if (p.size() == 0) { for (unsigned long i=0; i<n/2; ++i) { current[c[2*i]]=c[2*i+1]; current[c[2*i+1]]=c[2*i]; } return; }
			unsigned i=p[0];
			for (unsigned k=1; k<p.size(); ++k) {
				unsigned o = p[k];
				std::vector<unsigned> cc=c; cc.push_back(i); cc.push_back(o);
				std::vector<unsigned> pp; for (unsigned j : p) if ((j!=i) && (j!=o)) pp.push_back(j);
				todo_c.push_back(cc); todo_p.push_back(pp);
			}
		}
	}

	Pairings_Iterator::Pairings_Iterator (int n_, int i_, bool d) : current(n_), n(n_), i(i_) {
		if (d) {
			std::vector<unsigned> all; for (unsigned i=0; i<n; ++i) all.push_back(i);
			std::vector<unsigned> c;
			todo_c.push_back(c); todo_p.push_back(all);
			next();
		}
	}

	void Pairings_Iterator::operator++ () { ++i; next(); }

	bool Pairings_Iterator::operator!= (const Pairings_Iterator &o) const { return (n != o.n) || (i != o.i); }

	Permutation & Pairings_Iterator::operator* () { return current; }

	Pairings::Pairings (int n_) : n(n_)	{}

	unsigned Pairings::size () const {
		return fact(n)/fact(n/2)/pow(2,n/2);
	}

	Pairings_Iterator Pairings::begin () const { return Pairings_Iterator(n,0,true); }

	Pairings_Iterator Pairings::end () const { return Pairings_Iterator(n,size(),false); }

	Permutation Pairings::rand () {
		Permutation out(n);
		for (unsigned long i=0; i<n; ++i) {
			if (out[i]<i) continue;
			while (true) { out[i] = i+1+prng.uniform_int(n-i-1); if (out[out[i]]==out[i]) break; }
			out[out[i]] = i;
		}
		return out;
	}
}
