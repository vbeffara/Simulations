#include <vb/Cube.h>
#include <vb/PRNG.h>
#include <vb/CL_Parser.h>
#include <vb/ProgressBar.h>

using namespace vb; using namespace std;

class SRW3 : public Cube {
public:
	SRW3 (CL_Parser & CLP) : Cube (CLP('n'), CLP('n'), CLP('n'), "The 3d simple random walk"), n(CLP('n')), pos(n/2,n/2,n/2) {}
	void step () { pos = pos + dz3[prng.uniform_int(6)]; putp(pos,255); }

	int n; coo3 pos;
};

int main (int argc, char ** argv) {
	CL_Parser CLP (argc, argv, "n=100");
	SRW3 C (CLP);
	for (int t=0; t<C.n*C.n; ++t) C.step();
	C.output_pov();
}
