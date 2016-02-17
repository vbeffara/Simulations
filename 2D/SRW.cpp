#include <vb/coo.h>
#include <vb/Figure.h>
#include <vb/PRNG.h>

using namespace vb; using namespace std;

int main(int argc, char ** argv) {
	H.init ("Simple random walk", argc, argv, "a=15,o=1");
	int adj = H['a'];

	Figure f(H['o']); f.margin = 1;
	f.add (new Circle (0,.5)); f.add (new Path ({0}));
	f.show(); // f.pause();

	vector<cpx> & path = static_pointer_cast <Path> (f.contents[1]) -> z;
	cpx & z = static_pointer_cast <Circle> (f.contents[0]) -> z;

	for (int a=1; a<200; ++a) {
		for (int b=0; b<a; ++b) {
			int d=9; while (!((1<<d) & adj)) d = prng.uniform_int(8);
			z += cpx(dz[d]); path.push_back(z);
		}
		f.update();
	}
}
