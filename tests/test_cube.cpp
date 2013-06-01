#include <vb/Cube.h>
#include <vb/PRNG.h>
#include <vb/CL_Parser.h>

using namespace vb;

int main (int argc, char ** argv) {
	CL_Parser CLP (argc, argv, "n=100,b=1");
	double b = CLP('b'); b /= 255;

    Cube<unsigned char, int> C (CLP('n'),CLP('n'),CLP('n'),"3d Glauber");
    C.show();
    for (int x=0; x<C.sx; ++x) for (int y=0; y<C.sy; ++y) for (int z=0; z<C.sz; ++z) C.put(x,y,z, prng.bernoulli()?255:0);
	while (true) {
		int x=prng.uniform_int(C.sx);
		int y=prng.uniform_int(C.sy);
		int z=prng.uniform_int(C.sz);
		int S=0;
		S += C.atp(x+1,y,z); S += C.atp(x,y+1,z); S += C.atp(x,y,z+1);
		S += C.atp(x-1,y,z); S += C.atp(x,y-1,z); S += C.atp(x,y,z-1);
		C.put (x,y,z, prng.bernoulli(exp(b*S) / (exp(b*S) + exp(b*(6*255-S)))) ? 255 : 0);
	}
    return 0;
}
