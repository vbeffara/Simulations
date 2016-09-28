#include <vb/coo.h>
#include <vb/Figure.h>
#include <vb/PRNG.h>

using namespace vb; using namespace std;

int main(int argc, char ** argv) {
	H.init ("Simple random walk", argc, argv, "a=15,o=1");
	int adj = H['a'];

	auto C = new Circle(0,.5); cpx & z = C->z;
	auto P = new Path ({0}); vector<cpx> & path = P->z;
	Figure f(H['o']); f.add (C); f.add (P); f.margin = 1; f.show();

	for (int a=1; a<200; ++a) {
		for (int b=0; b<a; ++b) {
			int d=9; while (!((1<<d) & adj)) d = prng.uniform_int(8);
			z += cpx(dz[d]); path.push_back(z);
		}
		f.update();
	}
}
