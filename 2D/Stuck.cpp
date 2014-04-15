#include <vb/Hub.h>
#include <vb/Bitmap.h>

using namespace std; using namespace vb;

class Stat { public:
	Stat (int ss) : s(ss), ml(max) { max = std::max(max,s); }
	operator int () { return s; }
	operator Color() {
		if (s<0) return Color(0,64,0);
		if (s==0) return 0;
		if (ml<=.1*max) return Color(0,0,64);
		if (ml<=.9*max) return Color(0,0,64 + 128*(ml-.1*max)/(.8*max));
		return 64 + (s-min)*(255-64)/(max-min);
	}
	int s,ml;
	static int min,max;
};

int Stat::min=0, Stat::max=1;

class Stuck : public Bitmap<Stat> { public:
	Stuck (Hub H) : Bitmap<Stat> (2*int(H['n']),2*int(H['n']),H.title), alpha(H['a']), beta(H['b']) {
		for (int i=0; i<w()/2; ++i) for (int j=0; j<h()/2; ++j) at(coo(2*i,2*j))=-1;
	}
	void run() {
		coo z (w()/2,h()/2);
		vector<double> l(4);
		for (;;) {
			for (int i=0; i<4; ++i) {
				coo d=dz[i], dl=dz[(i+1)%4], dr=dz[(i+3)%4];
				l[i] = alpha * (atp(z+d+d+d) + atp(z+d+d+dl) + atp(z+d+d+dr)) - atp(z+d);
			}
			double ml=l[0], sl=0;
			for (int i=1; i<4; ++i) { ml = max (ml,l[i]); }
			for (int i=0; i<4; ++i) { l[i] = exp(beta*(l[i]-ml)); sl+=l[i]; }
			for (int i=0; i<4; ++i) { l[i] /= sl; }
			coo d = dz[prng.discrete(l)];
			putp(z+d,atp(z+d)+1);
			z += d+d;
		}
	}
	double alpha, beta;
};

int main (int argc, char ** argv) {
	Hub H ("Stuck walk on the square lattice",argc,argv,"n=200,a=.1432,b=5");
	Stuck S(H); S.show(); S.run();
}
