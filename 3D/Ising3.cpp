#include <vb/Cube.h>
#include <vb/PRNG.h>
#include <vb/CL_Parser.h>

using namespace vb; using namespace std;

class Ising3 : public Cube <unsigned char, int> { public: int b; double beta;
	Ising3 (CL_Parser & CLP) : Cube <unsigned char, int> (CLP('n'), CLP('n'), CLP('n'), "The 3d Ising model"), b(0), beta(CLP('b')) {
		beta /= 255;
		for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) put(coo3(x,y,z), prng.bernoulli() ? 255 : 0);
	};

	void bc_dobrushin () {
		b=1; for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) put(coo3(x,y,z), y<sy/2 ? 255 : 0);
	}

	void bc_cube () {
		for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) put(coo3(x,y,z), 0);
		for (int x=sx/4; x<3*sx/4; ++x) for (int y=sy/4; y<3*sy/4; ++y) for (int z=sz/4; z<3*sz/4; ++z) put(coo3(x,y,z), 255);
	}

	void swipe () {
		for (int t=0; t<sx*sy*sz; ++t) {
			int x=b+prng.uniform_int(sx-2*b), y=b+prng.uniform_int(sy-2*b), z=b+prng.uniform_int(sz-2*b), S=0;
			S += atp(coo3(x+1,y,z)); S += atp(coo3(x,y+1,z)); S += atp(coo3(x,y,z+1));
			S += atp(coo3(x-1,y,z)); S += atp(coo3(x,y-1,z)); S += atp(coo3(x,y,z-1));
			put (coo3(x,y,z), prng.bernoulli(exp(beta*S) / (exp(beta*S) + exp(beta*(6*255-S)))) ? 255 : 0);
		}
	}
};

int main (int argc, char ** argv) {
	CL_Parser CLP (argc, argv, "n=50,b=1");

    Ising3 C (CLP); C.show();
    C.bc_dobrushin(); for (int t=0; t<2*int(CLP('n')); ++t) C.swipe();
	C.output_pov();
}
