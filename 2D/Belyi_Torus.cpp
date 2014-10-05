#include <vb/ProgressBar.h>
#include <vb/Toroidal.h>
#include <cassert>

using namespace vb; using namespace std;

unsigned long long fact   	(unsigned long long n)                      	{ return (n<2 ? 1 : n*fact(n-1));     	}
unsigned long long binom  	(unsigned long long n, unsigned long long k)	{ return fact(n)/fact(k)/fact(n-k);   	}
unsigned long long catalan	(unsigned long long n)                      	{ return binom(2*n,n)/(n+1);          	}
unsigned long long npair  	(unsigned long long n)                      	{ return fact(n)/fact(n/2)/pow(2,n/2);	}

bool connected (Permutation s, Permutation a) {
	int n=s.size(); vector<int> l(n); for (int i=0; i<n; ++i) l[i]=i;
	bool dirty=true; while (dirty) {
		dirty=false;
		for (int i=0; i<n; ++i) if (l[s[i]]<l[i]) { l[i]=l[s[i]]; dirty=true; }
		for (int i=0; i<n; ++i) if (l[a[i]]<l[i]) { l[i]=l[a[i]]; dirty=true; }
	}
	for (int i=0; i<n; ++i) if (l[i]>0) return false;
	return true;
}

class Pairing_Iterator { public:
	Cycles                  	state_c;
	vector<vector<unsigned>>	state_p;
	vector<unsigned>        	state_k;

	void go () {
		if (state_c.size() == n/2) { l.push_back(state_c); pb->set(l.size()); return; }
		vector<unsigned> & p = state_p.back(); unsigned i=p[0];
		int k = state_k.back(); if (k==p.size()) return;
		unsigned o = p[k];
		vector<unsigned> new_p; for (unsigned j : p) if ((j!=i) && (j!=o)) new_p.push_back(j);
		state_c.push_back({i,o}); state_p.push_back(new_p); state_k.push_back(1);
		go();
		state_k.pop_back(); state_p.pop_back(); state_c.pop_back();
		state_k.push_back(k+1);
		go();
		state_k.pop_back();
	}

	Pairing_Iterator (int n_, int i_, bool d) : n(n_), i(i_) {
		if (d) {
			pb = new ProgressBar (npair(n));
			vector<unsigned> all; for (int i=0; i<n; ++i) all.push_back(i);
			state_p.push_back(all); state_k.push_back(1); go(); state_k.pop_back(); state_p.pop_back();
			delete pb;
		}
	};

	void         	operator++	()                   	{ ++i;                            	}
	bool         	operator!=	(Pairing_Iterator &o)	{ return (n != o.n) || (i != o.i);	}
	Permutation &	operator* 	()                   	{ return l[i];                    	}

	unsigned n,i;
	vector<Permutation> l;
	ProgressBar * pb;
};

class pairings { public:
	pairings              	(int n_) : n(n_)	{                                           	}
	Pairing_Iterator begin	()              	{ return Pairing_Iterator(n,0,true);        	};
	Pairing_Iterator end  	()              	{ return Pairing_Iterator(n,npair(n),false);	};
	int n;
};

int main (int argc, char ** argv) {
	Hub H ("Toroidal enumeration", argc, argv, "f=4,m=17");
	int F=H['f'], A=3*F/2, S=F/2, a=3*F; assert (F%2 == 0);

	Cycles phi_c; for (unsigned i=0; i<a/3; ++i) phi_c.push_back ({3*i,3*i+1,3*i+2}); Permutation phi(phi_c);

	int i=0; for (Permutation alpha : pairings(a)) {
		                                                                            	if (!connected(phi,alpha))	continue;
		Permutation sigma = (alpha*phi).inverse(); Hypermap M (sigma,alpha,phi);    	if (M.genus() != 1)       	continue;
		bool good=true; for (auto & c : sigma.cycles()) if (c.size()<=2) good=false;	if (!good)                	continue;

		// cout << "Sigma: " << sigma;
		// cout << "Alpha: " << alpha;
		// cout << "Phi:   " << phi;

		// ostringstream os; os << "Toroidal enumeration (f=" << F << ", i=" << i << ")"; H.title = os.str();
		// Toroidal T (M,H);
		// T.split_edges(); T.acpa(2.7);
		// T.split_edges(); T.acpa(2.7);
		// T.split_edges(); T.pack();

		// cout << "  " << T.m << endl;
		// cout << H.title << endl;
		// T.output_pdf();

		// cout << endl;
		++i;
	}

	cout << i << " triangulations." << endl;
}