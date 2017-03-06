#include <vb/Bitmap.h>
#include <vb/Console.h>

using namespace vb; using namespace std;

const vector<Color> C = { RED, BLUE, YELLOW, GREEN, MAGENTA, CYAN };

class Spin {
public:
	int s;
	Spin (int i) : s(i) {};
	operator Color() const { if ((s>=0)&&(s<6)) return C[s]; else if (s<0) return WHITE; else return Indexed(s); }
};

class Potts : public Bitmap<Spin> { public:
	Potts (Hub & H) : Bitmap<Spin> (int(H['n']), int(H['n'])), q(H['q']), beta(H['b']), b(0) {
		bcs["perio"] = []{};
		bcs["free"] = [this]{
			b=1; for (auto & c : *this) c=-1;
		};
		bcs["wired"] = [this]{
			b=1;
			for (int i=0; i<w(); ++i) { put (coo(i,0),1); put (coo(i,h()-1),1); }
			for (int i=0; i<h(); ++i) { put (coo(0,i),1); put (coo(w()-1,i),1); }
		};
		bcs["tripod"] = [this]{
			b=1;
			for (int j=0; j<h()-w()/2; ++j) {
				for (int i=0; i<w()/2; ++i)   put(coo(i,j),0);
				for (int i=w()/2; i<w(); ++i) put(coo(i,j),2);
			}
			for (int j=h()-w()/2; j<h(); ++j) {
				for (int i=0; i<h()-j; ++i)         put(coo(i,j),0);
				for (int i=h()-j; i<w()-h()+j; ++i) put(coo(i,j),1);
				for (int i=w()-h()+j; i<w(); ++i)   put(coo(i,j),2);
			}
		};
		bcs["quadripod"] = [this]{
			b=1;
			for (int j=0; j<h()/2; ++j) {
				for (int i=0; i<w()/2; ++i)   put(coo(i,j),0);
				for (int i=w()/2; i<w(); ++i) put(coo(i,j),1);
			}
			for (int j=h()/2; j<h(); ++j) {
				for (int i=0; i<w()/2; ++i)   put(coo(i,j),2);
				for (int i=w()/2; i<w(); ++i) put(coo(i,j),3);
			}
		};
		bcs["quadripod2"] = [this]{
			b=1;
			for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j) {
				if (i>j) { if (i+j<w()) put(coo(i,j),0); else put(coo(i,j),1); }
				else { if (i+j<w()) put(coo(i,j),2); else put(coo(i,j),3); }
			}
		};
		bcs["dobrushin"] = [this]{
			b=1;
			for (int i=0; i<w(); ++i) for (int j=0; j<h()/2; ++j)   put(coo(i,j),0);
			for (int i=0; i<w(); ++i) for (int j=h()/2; j<h(); ++j) put(coo(i,j),1);
		};
		bcs["loren"] = [this]{
			b=1;
			for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j)
				if ((i==0)||(j==0)||(i==w()-1)||(j==h()-1))
					put (coo(i,j),(q*(i+j)/w())%q);
		};
		bcs["loren2"] = [this]{ assert (q>=6);
			b=1;
			for (int i=0; i<w()/4; ++i)      	put(coo(i,0),0);
			for (int i=w()/4; i<3*w()/4; ++i)	put(coo(i,0),1);
			for (int i=3*w()/4; i<w(); ++i)  	put(coo(i,0),2);
			for (int i=0; i<h()/2; ++i)      	put(coo(0,i),0);
			for (int i=h()/2; i<h(); ++i)    	put(coo(0,i),5);
			for (int i=0; i<h()/2; ++i)      	put(coo(w()-1,i),2);
			for (int i=h()/2; i<h(); ++i)    	put(coo(w()-1,i),3);
			for (int i=0; i<w()/4; ++i)      	put(coo(i,h()-1),5);
			for (int i=w()/4; i<3*w()/4; ++i)	put(coo(i,h()-1),4);
			for (int i=3*w()/4; i<w(); ++i)  	put(coo(i,h()-1),3);
		};
		bcs["123"] = [this]{
			b=1; int c=0;
			for (int i=0; i<w()-1; ++i) { put(coo(i,0),c);          	c = (c+1)%q; }
			for (int i=0; i<h()-1; ++i) { put(coo(w()-1,i),c);      	c = (c+1)%q; }
			for (int i=0; i<w()-1; ++i) { put(coo(w()-1-i,h()-1),c);	c = (c+1)%q; }
			for (int i=0; i<h()-1; ++i) { put(coo(0,h()-1-i),c);    	c = (c+1)%q; }
		};
		bcs["12123333"] = [this]{
			b=1;
			for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j) put(coo(i,j), (j>h()/2) ? ((i+j)%2) : 2);
		};
		bcs["1231234444"] = [this]{
			b=1;
			for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j) put(coo(i,j), (j>h()/2) ? ((i+j)%3) : 3);
		};
		bcs["mostlyfree"] = [this]{
			b=1;
			for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j) put(coo(i,j),q);
		};

		for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j) put (coo(i,j), prng.uniform_int(q));
		beta *= log(1+sqrt(double(q)));
		bcs[H['c']](); show();
	}

	double H (Spin i, Spin j) { return i.s==j.s ? 0 : 1; }
	double H (coo z,  Spin i) { return H(i,atp(z+dz[0])) + H(i,atp(z+dz[1])) + H(i,atp(z+dz[2])) + H(i,atp(z+dz[3])); }

	void up (coo z) {
		int i = prng.uniform_int(q);
		double dH = H(z,i) - H(z,atp(z));
		if ((dH<=0) || prng.bernoulli(exp(-beta*dH))) put(z,i);
	}

	void up () { up (coo(b+prng.uniform_int(w()-2*b),b+prng.uniform_int(h()-2*b))); }

	int q;
	double beta;
	int b;
	map<string,function<void()>> bcs;
};

int main (int argc, char ** argv) {
	H.init ("Potts model", argc,argv, "n=500,q=3,b=1,c=free");
	Potts P(H);

	// Console W;
	// W.manage(P.beta,0.0,4.0,"beta");
	// W.show();

	while (true) P.up();
}
