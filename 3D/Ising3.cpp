#include <vb/Cube.h>
#include <vb/Hub.h>
#include <vb/PRNG.h>
#include <vb/ProgressBar.h>

using namespace vb; using namespace std;

class Ising3 : public Cube { public: int b; bool k; double beta; vector<double> glaub,kaw;
	Ising3 (int n, double k_, double b_, std::string c) : Cube (n, n, n, H.title), b(0), k(k_), beta(b_) {
		for (int k=0; k<=6; ++k)  glaub.push_back(exp(k*beta) / (exp(k*beta) + exp((6-k)*beta)));
		for (int k=0; k<=12; ++k) kaw.push_back(1/(1+exp(2*beta*(k-6))));
		if     	(c == "cube")     	bc_cube();
		else if	(c == "dobrushin")	bc_dobrushin();
		else if	(c == "step")     	bc_step();
		else if	(c == "hexagon")  	bc_hexagon();
		else   	                  	bc_bernoulli(H['p']);
	};

	void bc_bernoulli (double p=.5)	{ for (auto c = begin(); c != end(); ++c) put(c, prng.bernoulli(p)                      	? 255 : 0);	b=0; }
	void bc_dobrushin ()           	{ for (auto c = begin(); c != end(); ++c) put(c, c.y<sy/2                               	? 255 : 0);	b=1; }
	void bc_step ()                	{ for (auto c = begin(); c != end(); ++c) put(c, (c.y<sy/2) || ((c.y==sy/2)&&(c.z>sz/2))	? 255 : 0);	b=1; }
	void bc_cube ()                	{ for (auto c = begin(); c != end(); ++c) put(c, 0); b=0;
	    for (int x=sx/10; x<9*sx/10; ++x) for (int y=sy/10; y<9*sy/10; ++y) for (int z=sz/10; z<9*sz/10; ++z) put(coo3(x,y,z), 255);
	}
	void bc_hexagon () { b=1; for (int i=0; i<sx; ++i) for (int j=0; j<sx; ++j) {
		put (coo3(i,j,sz-1), 255); put (coo3(0,i,j), 255); put (coo3(i,0,j), 255);
	}}

	int nbsum (coo3 c) { int S=0; for (int i=0; i<6; ++i) S += atp(c+dz3[i]) ? 1 : 0; return S; }

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

int main (int argc, char ** argv) {
	H.init ("3D Ising model", argc,argv, "n=50,b=1,t=0,p=.5,k,c=bernoulli,s=0,m,i");
    Ising3 C (H['n'], H['k'], H['b'], H['c']); C.show();
	int T = H['t']; if (T==0) T = 2*int(H['n']);
    { ProgressBar P (T); int s = H['s']; for (int t=0; t<T; ++t) {
		if (s && (t % (T/s)) == 0) {
			std::ostringstream os; os << "snapshot_" << std::setw(4) << std::setfill('0') << t/(T/s);
			C.output_pov (os.str());
		}
		C.swipe(); P.set(t);
	} }
	if (H['m']) C.mirrors=false;
	if (H['i']) for (auto c = C.begin(); c != C.end(); ++c) C.put(c, (c.y>C.sy/2) ? C.at(c)/2 : C.at(c));

	C.output_pov();
}
