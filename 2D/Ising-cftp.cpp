#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace vb;
using namespace std;

class Site {
public:
	Site (int _s) : s(_s) {}
	operator Color () { switch (s) { case 0 : return BLACK; case 1 : return RED; } return WHITE; }
	int s;
};

class IsingCFTP : public Bitmap<Site> {
public:
	IsingCFTP (CL_Parser & CLP) : Bitmap<Site> (CLP('n'), CLP('n'), "CFTP for Ising"), b(CLP('b')), d(0), s(CLP('s')),
								  status (w(),h(),"current status") {
		for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j) put (coo(i,j),1);
		b *= log(1+sqrt(double(2)));
		for (int i=0; i<=4; ++i) p.push_back (exp(b*i) / (exp(b*i) + exp(b*(4-i))));
	};

	void up (coo z) {
		double U = prng.uniform_real(); int n1=0, n2=0;
		for (int i=0; i<4; ++i) { if (atp(z+dz[i]).s>=1) ++n1; if (atp(z+dz[i]).s>=2) ++n2; }
		if (U<p[n2]) put(z,2); else if (U<p[n1]) put(z,1); else put(z,0);
	}

	void up ()  	{ for (int i=0; i<w()*h(); ++i) up(coo(d+prng.uniform_int(w()-2*d),d+prng.uniform_int(h()-2*d))); }
	void snap ()	{ for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j) status.put (coo(i,j),at(coo(i,j))); status.update(); }
	void show ()	{ Bitmap<Site>::show(); snap(); status.show(); }

	void run () {
		vector<string> states;
		int n = w()*h();
		while (n>0) {
			cerr << n << endl;
			states.push_back(prng.state());
			for (int i=d; i<w()-2*d; ++i) for (int j=d; j<h()-2*d; ++j) put (coo(i,j),1);
			for (int t=states.size()-1; t>=0; --t) {
				prng.state(states[t]);
				for (int i=0; i<(1<<t); ++i) up();
			}
			snap(); if(s) status.snapshot();
			n=0; for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j) if (at(coo(i,j)).s==1) ++n;
		}
	}

	void bc_0 () { d=1;
		for (int i=0; i<w(); ++i) { put (coo(i,0),0); put (coo(i,h()-1),0); }
		for (int j=0; j<h(); ++j) { put (coo(0,j),0); put (coo(w()-1,j),0); }
	}

	void bc_dobrushin () { d=1;
		for (int i=0; i<w(); ++i) { put (coo(i,0),0); put (coo(i,h()-1),2); }
		for (int j=0; j<h()/2; ++j) { put (coo(0,j),0); put (coo(w()-1,j),0); }
		for (int j=h()/2; j<h(); ++j) { put (coo(0,j),2); put (coo(w()-1,j),2); }
	}

	double b;
	int d;
	bool s;
	Image status;
	vector<double> p;
};

int main(int argc, char *argv[]) {
	CL_Parser CLP (argc,argv,"n=200,b=1,s");
	IsingCFTP I (CLP); I.bc_dobrushin();
	I.show(); I.run(); I.pause();
}
