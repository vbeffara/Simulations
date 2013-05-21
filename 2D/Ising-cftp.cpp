#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace vb;
using namespace std;

class Site {
public:
	Site (int _s) : s(_s) {}
	operator Color () { switch (s) { case 0 : return BLACK; case 1 : return GREEN; } return WHITE; }
	int s;
};

class IsingCFTP : public Bitmap<Site> {
public:
	IsingCFTP (CL_Parser & CLP) : Bitmap<Site> (CLP('n'), CLP('n'), "CFTP for Ising"), b(CLP('b')), status (w(),h(),"current status") {
		for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j) put (coo(i,j),1);
		b *= log(1+sqrt(double(2)));
		snap();
	};

	void up (coo z) {
		double U = prng.uniform_real(); int n1=0, n2=0;
		for (int i=0; i<4; ++i) { if (atp(z+dz[i]).s>=1) ++n1; if (atp(z+dz[i]).s>=2) ++n2; }
		double p1 = exp(b*n1) / (exp(b*n1) + exp(b*(4-n1)));
		double p2 = exp(b*n2) / (exp(b*n2) + exp(b*(4-n2)));
		if (U<p2) put(z,2); else if (U<p1) put(z,1); else put(z,0);
	}

	void up () { int x=prng.uniform_int(w()), y=prng.uniform_int(h()); up(coo(x,y)); }

	void show () { Bitmap<Site>::show(); status.show(); }

	void snap () { for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j) status.put (coo(i,j),at(coo(i,j))); }

	void run () {
		vector<string> states;
		int T=0; states.push_back(prng.state());
		bool finished = false;
		while (!finished) {
			for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j) put (coo(i,j),1);
			for (int t=T; t>=0; --t) {
				prng.state(states[t]);
				for (int i=0; i<w()*h()*(1<<t); ++i) up();
			}
			snap();
			states.push_back(prng.state()); ++T;
			int n=0; for (int i=0; i<w(); ++i) for (int j=0; j<h(); ++j) if (at(coo(i,j)).s==1) ++n;
			cerr << n << endl; finished = n==0;
		}
	}

	double b;
	Image status;
};

int main(int argc, char *argv[]) {
	CL_Parser CLP (argc,argv,"n=400,b=1");
	IsingCFTP I (CLP);
	I.show();
	I.run();
}
