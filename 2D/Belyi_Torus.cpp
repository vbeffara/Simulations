#include <vb/PRNG.h>
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
	vector<vector<unsigned>> todo_c, todo_p;
	Permutation current;

	void next () {
		while (todo_c.size()>0) {
			vector<unsigned> c = todo_c.back(); todo_c.pop_back();
			vector<unsigned> p = todo_p.back(); todo_p.pop_back();
			if (p.size() == 0) { for (int i=0; i<n/2; ++i) { current[c[2*i]]=c[2*i+1]; current[c[2*i+1]]=c[2*i]; } return; }
			unsigned i=p[0];
			for (int k=1; k<p.size(); ++k) {
				unsigned o = p[k];
				vector<unsigned> cc=c; cc.push_back(i); cc.push_back(o);
				vector<unsigned> pp; for (unsigned j : p) if ((j!=i) && (j!=o)) pp.push_back(j);
				todo_c.push_back(cc); todo_p.push_back(pp);
			}
		}
	}

	Pairing_Iterator (int n_, int i_, bool d) : current(n_), n(n_), i(i_) {
		if (d) {
			vector<unsigned> all; for (int i=0; i<n; ++i) all.push_back(i);
			vector<unsigned> c;
			todo_c.push_back(c); todo_p.push_back(all);
			next();
		}
	};

	void         	operator++	()                   	{ ++i; next();                    	}
	bool         	operator!=	(Pairing_Iterator &o)	{ return (n != o.n) || (i != o.i);	}
	Permutation &	operator* 	()                   	{ return current;                 	}

	unsigned n,i;
	ProgressBar * pb;
};

class pairings { public:
	int n;

	pairings        	     	(int n_) : n(n_)	{                                           	}
	Pairing_Iterator	begin	()              	{ return Pairing_Iterator(n,0,true);        	};
	Pairing_Iterator	end  	()              	{ return Pairing_Iterator(n,npair(n),false);	};

	Permutation rand () {
		Permutation out(n);
		for (int i=0; i<n; ++i) {
			if (out[i]<i) continue;
			while (true) { out[i] = i+1+prng.uniform_int(n-i-1); if (out[out[i]]==out[i]) break; }
			out[out[i]] = i;
		}
		return out;
	}
};

Permutation relabel (Permutation a, Permutation b, unsigned i) {
	int n = a.size(), m = 0;
	vector<unsigned> s1(n,n), s2(n,n);
	vector<unsigned> stack; stack.push_back(i); s1[i]=0; s2[0]=i; ++m;
	while (stack.size()) {
		int x = stack.back(); stack.pop_back();
		if (s1[a[x]]==n) { stack.push_back(a[x]); s1[a[x]] = m; s2[m]=a[x]; ++m; }
		if (s1[b[x]]==n) { stack.push_back(b[x]); s1[b[x]] = m; s2[m]=b[x]; ++m; }
	}
	return s1;
}

int main (int argc, char ** argv) {
	Hub H ("Toroidal enumeration", argc, argv, "f=4,m=17");
	int F=H['f'], A=3*F/2, S=F/2, a=3*F; assert (F%2 == 0);

	Cycles phi_c; for (unsigned i=0; i<a/3; ++i) phi_c.push_back ({3*i,3*i+1,3*i+2}); Permutation phi(phi_c);

	vector<Hypermap> v;

	int i=0; for (Permutation alpha : pairings(a)) {
		                                                                            	if (!connected(phi,alpha))	continue;
		Permutation sigma = (alpha*phi).inverse(); Hypermap M (sigma,alpha,phi);    	if (M.genus() != 1)       	continue;
		bool good=true; for (auto & c : sigma.cycles()) if (c.size()<=2) good=false;	if (!good)                	continue;

		cout << "Sigma: " << sigma;
		cout << "Alpha: " << alpha;
		cout << "Phi:   " << phi;

		Permutation s = relabel (alpha,phi,0);
		Hypermap B (sigma.conjugate(s),alpha.conjugate(s),phi.conjugate(s));
		bool there = false; for (Hypermap & O : v) if (O==B) there = true;
		if (!there) v.push_back(B);

		// ostringstream os; os << "Toroidal enumeration (f=" << F << ", i=" << i << ")"; H.title = os.str();
		// Toroidal T (M,H);
		// T.split_edges(); T.acpa(2.7);
		// T.split_edges(); T.acpa(2.7);
		// T.split_edges(); T.pack();

		// cout << "  " << T.m << endl;
		// cout << H.title << endl;
		// T.output_pdf();

		cout << endl;
		++i;
	}

	cout << i << " triangulations, " << v.size() << " unique ones." << endl << endl;

	for (Hypermap M : v) {
		cout << "| Sigma: " << M.sigma;
		cout << "| Alpha: " << M.alpha;
		cout << "| Phi:   " << M.phi;
		cout << endl;
	}
}