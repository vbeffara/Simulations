#include <vb/Cube.h>
#include <vb/PRNG.h>
#include <vb/CL_Parser.h>
#include <vb/Pov.h>

using namespace vb;
using namespace std;

int main (int argc, char ** argv) {
	CL_Parser CLP (argc, argv, "n=50,b=1");
	double beta = CLP('b'); beta /= 255;
	int b=1;

    Cube<unsigned char, int> C (CLP('n'),CLP('n'),CLP('n'),"3d Glauber");
	int dist = C.sx+C.sy+C.sz;

    C.show();
    for (int x=0; x<C.sx; ++x) for (int y=0; y<C.sy; ++y) for (int z=0; z<C.sz; ++z) C.put(x,y,z, y<C.sy/2 ? 255 : 0);
	for (int t=0; t<10*dist*dist*dist; ++t) {
		int x=b+prng.uniform_int(C.sx-2*b);
		int y=b+prng.uniform_int(C.sy-2*b);
		int z=b+prng.uniform_int(C.sz-2*b);
		int S=0;
		S += C.atp(x+1,y,z); S += C.atp(x,y+1,z); S += C.atp(x,y,z+1);
		S += C.atp(x-1,y,z); S += C.atp(x,y-1,z); S += C.atp(x,y,z-1);
		C.put (x,y,z, prng.bernoulli(exp(beta*S) / (exp(beta*S) + exp(beta*(6*255-S)))) ? 255 : 0);
	}

	std::cout	<< "#version 3.7;" << std::endl << "#include \"colors.inc\"" << std::endl << "#include \"stones.inc\"" << std::endl
	         	<< "background { color White }" << std::endl
	         	<< new Pov_Camera (tri(7*C.sx/4,1.3*C.sy,-C.sz), tri(C.sx/2,C.sy/2,C.sz/2), 60)
	         	<< new Pov_Light_Source (tri(1.25*C.sx,1.8*C.sy,-2.0*C.sz))
	         	<< new Pov_Plane (tri(1,0,0), -C.sx/4) << new Pov_Plane (tri(0,1,0), -C.sy/8) << new Pov_Plane (tri(0,0,1), 5*C.sz/4)
	         	<< new Pov_Frame (tri(0,0,0), tri(C.sx,C.sy,C.sz));
	for (int x=0; x<C.sx; ++x) for (int y=0; y<C.sy; ++y) for (int z=0; z<C.sz; ++z) if (C.at(x,y,z)==255)
		std::cout << new Pov_Box (tri(x,y,z), tri(x+1,y+1,z+1));
    return 0;
}
