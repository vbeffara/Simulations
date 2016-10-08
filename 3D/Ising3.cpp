#include <vb/Cube.h>
#include <vb/ProgressBar.h>

using namespace vb; using namespace std;

class Ising3 : public Cube { public: int b; bool k; double beta; vector<double> glaub,kaw;
	Ising3 (int n, double k_, double b_) : Cube (n, n, n), b(0), k(k_), beta(b_) {
		for (int k=0; k<=6; ++k)  glaub.push_back(exp(k*beta) / (exp(k*beta) + exp((6-k)*beta)));
		for (int k=0; k<=12; ++k) kaw.push_back(1/(1+exp(2*beta*(k-6))));
	};

	long nbsum (coo3 c) { int S=0; for (int i=0; i<6; ++i) S += atp(c+dz3[i]) ? 1 : 0; return S; }

	void spin (coo3 c) {
		if (k) {
			int d = prng.uniform_int(6); coo3 z2 = c+dz3[d]; if (at(c) == atp(z2)) return;
			if (prng.bernoulli(kaw[nbsum(z2)-nbsum(c)+6])) put(c,255), putp(z2,0); else put(c,0), putp(z2,255);
		} else {
			put (c, prng.bernoulli(glaub[nbsum(c)]) ? 255 : 0);
		}
	}

	void swipe () { for (int t=0; t<sx*sy*sz; ++t) { coo3 c = rand(b); spin(c); } }
};

class BCs : public map <string, function<void(Ising3&)>> { public:
	BCs () {
		emplace ("bernoulli", [](Ising3 &I) { I.b=0; for (auto c = I.begin(); c != I.end(); ++c) if (prng.bernoulli(H['p'])) I.put(c,255); } );
		emplace ("dobrushin", [](Ising3 &I) { I.b=1; for (auto c = I.begin(); c != I.end(); ++c) if (c.y<I.sy/2) I.put(c,255); } );
		emplace ("step", [](Ising3 &I) { I.b=1; for (auto c = I.begin(); c != I.end(); ++c) if ((c.y<I.sy/2) || ((c.y==I.sy/2)&&(c.z>I.sz/2))) I.put(c,255); } );
		emplace ("111",  [](Ising3 &I) { I.b=1; for (auto c = I.begin(); c != I.end(); ++c) if (c.x+c.y+(I.sz-c.z) < (I.sx+I.sy+I.sz)/2) I.put(c,255); } );
		emplace ("hexagon", [](Ising3 &I) { I.b=1; for (int i=0; i<I.sx; ++i) for (int j=0; j<I.sx; ++j) {
			I.put (coo3(i,j,I.sz-1), 255); I.put (coo3(0,i,j), 255); I.put (coo3(i,0,j), 255);
		}} );
		emplace ("cube", [](Ising3 &I) { I.b=0;
		    for (int x=I.sx/10; x<9*I.sx/10; ++x) for (int y=I.sy/10; y<9*I.sy/10; ++y) for (int z=I.sz/10; z<9*I.sz/10; ++z) I.put(coo3(x,y,z), 255);
		} );
		emplace ("slope", [](Ising3 &I) { I.b=1;
			for (auto c = I.begin(); c != I.end(); ++c) if ((c.y-I.sy/2) < double(H['p'])*(c.z-I.sz/2)) I.put(c,255);
		} );
	}
};

int main (int argc, char ** argv) {
	H.init ("3D Ising model", argc,argv, "n=50,b=1,t=0,p=.5,k,c=bernoulli,s=0,m,i");
    Ising3 C (H['n'], H['k'], H['b']); BCs()[H['c']] (C); C.show();
	int T = H['t']; if (T==0) T = 2*int(H['n']);
    { ProgressBar P (T); int s = H['s']; for (int t=0; t<T; ++t) {
		if (s && (t % (T/s)) == 0) C.output_pov (fmt::format ("snapshots/snapshot_{:04d}",t/(T/s)));
		C.swipe(); P.set(t);
	} }
	if (H['m']) C.mirrors=false;
	if (H['i']) for (auto c = C.begin(); c != C.end(); ++c) C.put(c, (c.y>C.sy/2) ? C.at(c)/2 : C.at(c));

	C.output_pov();
}
