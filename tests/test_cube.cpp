#include <vb/Cube.h>
#include <vb/PRNG.h>
#include <vb/CL_Parser.h>

using namespace vb;

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

	std::cout	<< "#include \"colors.inc\"" << std::endl
	         	<< "#include \"stones.inc\"" << std::endl
	         	<< "camera { location <" << 1.75*C.sx << "," << 5*C.sy/4 << "," << -C.sz
	         		<< "> look_at <" << C.sx/2 << "," << C.sy/2 << "," << C.sz/2 << "> angle 60 }" << std::endl
	         	<< "light_source { <" << C.sx/4 << "," << 3*C.sy/2 << "," << -2*C.sz << "> color White*2 }" << std::endl
	         	<< "plane { x, " << - C.sx/4 << " texture { pigment { color White } } finish { reflection {.6} ambient 0.2 diffuse 0 } }" << std::endl
	         	<< "plane { y, " << - C.sy/4 << " texture { pigment { color White } } finish { reflection {.4} ambient 0.2 diffuse 0.4 } }" << std::endl
	         	<< "plane { z, " << 5*C.sz/4 << " texture { pigment { color White } } finish { reflection {.6} ambient 0.2 diffuse 0 } }" << std::endl
	         	<< "cylinder { <0,0,0>, <" << C.sx << ",0,0>, .1 texture { pigment { color Red } } }" << std::endl
	         	<< "cylinder { <0,0,0>, <0," << C.sy << ",0>, .1 texture { pigment { color Red } } }" << std::endl
	         	<< "cylinder { <0," << C.sy << ",0>, <" << C.sx << "," << C.sy << ",0>, .1 texture { pigment { color Red } } }" << std::endl
	         	<< "cylinder { <" << C.sx << ",0,0>, <" << C.sx << "," << C.sy << ",0>, .1 texture { pigment { color Red transmit .5 } } }" << std::endl
	         	<< "cylinder { <" << C.sx << "," << C.sy << "," << C.sz << ">, <" << C.sx << "," << C.sx << ",0>, .1 texture { pigment { color Red } } }" << std::endl
	         	<< "cylinder { <0,0,0>, <0,0," << C.sz << ">, .1 texture { pigment { color Red } } }" << std::endl
	         	<< "background { color White }" << std::endl;
	for (int x=0; x<C.sx; ++x)
		for (int y=0; y<C.sy; ++y)
			for (int z=0; z<C.sz; ++z)
				if (C.at(x,y,z)==255)
					std::cout << "box { <" << x << "," << y << "," << z << ">,<" << x+1 << "," << y+1 << "," << z+1
							<< "> texture { pigment { color rgb <.5,.7,1> } } }" << std::endl;
    return 0;
}
