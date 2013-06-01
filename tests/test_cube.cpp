#include <vb/Cube.h>
#include <vb/PRNG.h>
#include <vb/CL_Parser.h>

using namespace vb;

int main (int argc, char ** argv) {
	CL_Parser CLP (argc, argv, "n=50,b=1");
	double b = CLP('b'); b /= 255;

    Cube<unsigned char, int> C (CLP('n'),CLP('n'),CLP('n'),"3d Glauber");
	int dist = C.sx+C.sy+C.sz;

    C.show();
    for (int x=0; x<C.sx; ++x) for (int y=0; y<C.sy; ++y) for (int z=0; z<C.sz; ++z) C.put(x,y,z, prng.bernoulli()?255:0);
	for (int t=0; t<10*dist*dist*dist; ++t) {
		int x=prng.uniform_int(C.sx);
		int y=prng.uniform_int(C.sy);
		int z=prng.uniform_int(C.sz);
		int S=0;
		S += C.atp(x+1,y,z); S += C.atp(x,y+1,z); S += C.atp(x,y,z+1);
		S += C.atp(x-1,y,z); S += C.atp(x,y-1,z); S += C.atp(x,y,z-1);
		C.put (x,y,z, prng.bernoulli(exp(b*S) / (exp(b*S) + exp(b*(6*255-S)))) ? 255 : 0);
	}


	std::cout	<< "#include \"colors.inc\"" << std::endl
	         	<< "#include \"stones.inc\"" << std::endl
	         	<< "camera { location <" << dist << "," << dist << "," << -dist
	         		<< "> look_at <" << C.sx/2 << "," << C.sy/2 << "," << C.sz/2 << "> angle 25 }" << std::endl
	         	<< "light_source { <" << dist << "," << 2*dist << "," << -2*dist << "> color White }" << std::endl
	         	<< "background { color White }" << std::endl;
	for (int x=0; x<C.sx; ++x)
		for (int y=0; y<C.sy; ++y)
			for (int z=0; z<C.sz; ++z)
				if (C.at(x,y,z)==255)
					std::cout << "box { <" << x << "," << y << "," << z << ">,<" << x+1 << "," << y+1 << "," << z+1
							<< "> texture {T_Stone1} }" << std::endl;
    return 0;
}
