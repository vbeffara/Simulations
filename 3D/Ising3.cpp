#include <vb/Cube.h>
#include <vb/PRNG.h>
#include <vb/CL_Parser.h>
#include <vb/Pov.h>

using namespace vb; using namespace std;

class Ising3 : public Cube <unsigned char, int> { public: int b; double beta;
	Ising3 (CL_Parser & CLP) : Cube <unsigned char, int> (CLP('n'), CLP('n'), CLP('n'), "The 3d Ising model"), b(0), beta(CLP('b')) {
		beta /= 255;
		for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) put(x,y,z, prng.bernoulli() ? 255 : 0);
	};

	void bc_dobrushin () {
		b=1; for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) put(x,y,z, y<sy/2 ? 255 : 0);
	}

	void bc_cube () {
		for (int x=0; x<sx; ++x) for (int y=0; y<sy; ++y) for (int z=0; z<sz; ++z) put(x,y,z, 0);
		for (int x=sx/4; x<3*sx/4; ++x) for (int y=sy/4; y<3*sy/4; ++y) for (int z=sz/4; z<3*sz/4; ++z) put(x,y,z, 255);
	}

	void swipe () {
		for (int t=0; t<10*sx*sy*sz; ++t) {
			int x=b+prng.uniform_int(sx-2*b), y=b+prng.uniform_int(sy-2*b), z=b+prng.uniform_int(sz-2*b), S=0;
			S += atp(x+1,y,z); S += atp(x,y+1,z); S += atp(x,y,z+1);
			S += atp(x-1,y,z); S += atp(x,y-1,z); S += atp(x,y,z-1);
			put (x,y,z, prng.bernoulli(exp(beta*S) / (exp(beta*S) + exp(beta*(6*255-S)))) ? 255 : 0);
		}
	}
};

int main (int argc, char ** argv) {
	CL_Parser CLP (argc, argv, "n=50,b=1");

    Ising3 C (CLP); C.show();
    C.bc_cube(); for (int t=0; t<10; ++t) C.swipe();

	Pov_Scene S; S	<< new Pov_Camera      	(tri(7*C.sx/4,1.3*C.sy,-C.sz), tri(C.sx/2,C.sy/2,C.sz/2), 60)
	              	<< new Pov_Light_Source	(tri(1.25*C.sx,1.8*C.sy,-2.0*C.sz))
	              	<< new Pov_Plane       	(tri(1,0,0), -C.sx/4)
	              	<< new Pov_Plane       	(tri(0,1,0), -C.sy/8)
	              	<< new Pov_Plane       	(tri(0,0,1), 5*C.sz/4)
	              	<< new Pov_Frame       	(tri(0,0,0), tri(C.sx,C.sy,C.sz));
	for (int x=0; x<C.sx; ++x) for (int y=0; y<C.sy; ++y) for (int z=0; z<C.sz; ++z) if (C.at(x,y,z)==255)
		S << new Pov_Box (tri(x,y,z), tri(x+1,y+1,z+1));

	std::cout << S; return 0;
}
