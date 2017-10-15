#include <vb/Bitmap.h>

using namespace std; using namespace vb;

template<> Color vb::to_Color (int t) { return t==0 ? BLACK : Indexed(t); }

class ACP : public Bitmap<int> { public:
	explicit ACP (const Hub & H) : Bitmap<int> (int(H['n']), int(H['n'])), kid(H['z']), maxage (H['m']) {
		put (coo(w()/2,h()/2),1);
		P = { H['d'], H['a'] };
		for (int i=0; i<int(H['m']) - kid; ++i) P.push_back (double(H['b']) + double(H['r'])*i);
		double s=0; for (double u : P) s+=u; for (double &u : P) u /= s;
	}
	void run() {
		coo z = rand(); if (at(z)==0) return;
		int action = prng.discrete (P);
		if (action == 0)                           	{ put (z, 0); }
		else if ((action == 1) && (at(z) < maxage))	{ put (z, at(z)+1); }
		else if (action-1+kid <= int(at(z)))       	{ coo nz = z + dz[prng.uniform_int(4)]; if (atp(nz) == 0) putp(nz,1); }
	}
	int kid, maxage;
	vector<double> P;
};

int main (int argc, char ** argv) {
	H.init ("Contact process with aging", argc, argv, "n=400,d=.01,a=.1,z=0,m=5,b=.2,r=.1");
	ACP A (H); A.show(); while (true) A.run();
}
