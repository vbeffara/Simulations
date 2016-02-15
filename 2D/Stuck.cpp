#include <vb/Bitmap.h>
#include <vb/Console.h>

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
	Stuck (Hub H) : Bitmap<Stat> (2*int(H['n']),2*int(H['n'])), alpha(H['a']), beta(H['b']) {
		for (int i=0; i<w()/2; ++i) for (int j=0; j<h()/2; ++j) at(coo(2*i,2*j))=-1;
		C.watch (alpha, "Alpha");
		C.manage (alpha, 0.142, 0.35);
		C.lambda ((std::function<int()>) ([this](){return this->nsup();}), "Support");
	}
	void show () { C.show(); Bitmap<Stat>::show(); }
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
	void update () {
		int m=Stat::max;
		for (int v : *this) if (v > 0) m = min (m,v);
		if (m<Stat::max) Stat::min=m;
		Bitmap<Stat>::update();
	}
	int nsup () { int n=0; for (Stat v : *this) if ((v.s>0) && (v.ml>.9*Stat::max)) ++n; return n; }

	double alpha, beta;
	Console C;
};

int main (int argc, char ** argv) {
	H.init ("Stuck walk on the square lattice",argc,argv,"n=200,a=.1432,b=5,v=0");
	Stuck S(H); S.show();
	if (double(H['v'])>0) S.snapshot_setup("Stuck",H['v']);
	S.run();
}
