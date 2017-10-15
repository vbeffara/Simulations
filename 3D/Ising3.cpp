#include <vb/Cube.h>
#include <vb/ProgressBar.h>

class Ising3 : public vb::Cube { public: int b; bool k; double beta; std::vector<double> glaub,kaw;
	Ising3 (int n, bool k_, double b_) : Cube (n, n, n), b(0), k(k_), beta(b_) {
		for (int k=0; k<=6; ++k)  glaub.push_back(exp(k*beta) / (exp(k*beta) + exp((6-k)*beta)));
		for (int k=0; k<=12; ++k) kaw.push_back(1/(1+exp(2*beta*(k-6))));
	};

	int64_t nbsum (vb::coo3 c) { int S=0; for (int i=0; i<6; ++i) S += atp(c+vb::dz3[i])!=0 ? 1 : 0; return S; }

	void spin (vb::coo3 c) {
		if (k) {
			int d = vb::prng.uniform_int(6); vb::coo3 z2 = c+vb::dz3[d]; if (at(c) == atp(z2)) return;
			if (vb::prng.bernoulli(kaw[nbsum(z2)-nbsum(c)+6])) put(c,255), putp(z2,0); else put(c,0), putp(z2,255);
		} else {
			put (c, vb::prng.bernoulli(glaub[nbsum(c)]) ? 255 : 0);
		}
	}

	void swipe () { for (int t=0; t<sx*sy*sz; ++t) { vb::coo3 c = rand(b); spin(c); } }
};

class BCs : public std::map <std::string, std::function<void(Ising3&)>> { public:
	BCs () {
		emplace ("bernoulli", [](Ising3 &I) {
			I.b=0; for (auto c = I.begin(); c != I.end(); ++c)
				if (vb::prng.bernoulli(vb::H['p'])) { const vb::coo3 &cc = c; I.put(cc,255); }
		} );
		emplace ("dobrushin", [](Ising3 &I) { I.b=1; for (auto c = I.begin(); c != I.end(); ++c) if (c.y<I.sy/2) I.put(c,255); } );
		emplace ("step", [](Ising3 &I) { I.b=1; for (auto c = I.begin(); c != I.end(); ++c) if ((c.y<I.sy/2) || ((c.y==I.sy/2)&&(c.z>I.sz/2))) I.put(c,255); } );
		emplace ("111",  [](Ising3 &I) { I.b=1; for (auto c = I.begin(); c != I.end(); ++c) if (c.x+c.y+(I.sz-c.z) < (I.sx+I.sy+I.sz)/2) I.put(c,255); } );
		emplace ("hexagon", [](Ising3 &I) { I.b=1; for (int i=0; i<I.sx; ++i) for (int j=0; j<I.sx; ++j) {
			I.put (vb::coo3(i,j,I.sz-1), 255); I.put (vb::coo3(0,i,j), 255); I.put (vb::coo3(i,0,j), 255);
		}} );
		emplace ("cube", [](Ising3 &I) { I.b=0;
		    for (int x=I.sx/10; x<9*I.sx/10; ++x) for (int y=I.sy/10; y<9*I.sy/10; ++y) for (int z=I.sz/10; z<9*I.sz/10; ++z) I.put(vb::coo3(x,y,z), 255);
		} );
		emplace ("slope", [](Ising3 &I) { I.b=1;
			for (auto c = I.begin(); c != I.end(); ++c) if ((c.y-I.sy*.5) - double(vb::H['p'])*(c.z-I.sz*.5) + double(vb::H['q'])*(c.x-I.sx*.5) < 0) I.put(c,255);
		} );
	}
};

int main (int argc, char ** argv) {
	vb::H.init ("3D Ising model", argc,argv, "n=50,b=1,t=0,p=.5,q=0,k,c=bernoulli,s=0,m,i");
    Ising3 C (vb::H['n'], vb::H['k'], vb::H['b']); BCs()[vb::H['c']] (C); C.show();
	int T = vb::H['t']; if (T==0) T = 2*int(vb::H['n']);
    { vb::ProgressBar P (T); int s = vb::H['s']; for (int t=0; t<T; ++t) {
		if ((s>0) && (t % (T/s)) == 0) C.output_pov (fmt::format ("snapshots/snapshot_{:04d}",t/(T/s)));
		C.swipe(); P.set(t);
	} }
	if (vb::H['m']) C.mirrors=false;
	if (vb::H['i']) for (auto c = C.begin(); c != C.end(); ++c) C.put(c, (c.y>C.sy/2) ? C.at(c)/2 : C.at(c));

	C.output_pov();
}
