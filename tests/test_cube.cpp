#include <vb/Cube.h>
#include <vb/PRNG.h>

using namespace vb;

int main (int argc, char ** argv) {
    Cube<unsigned char, int> C (50,100,200,"3d Glauber");
    C.show();
    for (int x=0; x<C.sx; ++x) for (int y=0; y<C.sy; ++y) for (int z=0; z<C.sz; ++z) C.put(x,y,z, prng.bernoulli()?255:0);
	while (true) {
		int x=prng.uniform_int(C.sx);
		int y=prng.uniform_int(C.sy);
		int z=prng.uniform_int(C.sz);
		int S=0;
		S += C.atp(x+1,y,z); S += C.atp(x,y+1,z); S += C.atp(x,y,z+1);
		S += C.atp(x-1,y,z); S += C.atp(x,y-1,z); S += C.atp(x,y,z-1);
		if (S>3*255)     	C.put (x,y,z,255);
		else if (S<3*255)	C.put (x,y,z,0);
		else             	C.put (x,y,z, prng.bernoulli() ? 255 : 0);
	}
    return 0;
}
