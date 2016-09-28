#include <vb/coo.h>
#include <vb/Figure.h>
#include <vb/PRNG.h>

using namespace vb; using namespace std;

int main(int argc, char ** argv) {
	H.init ("Simple random walk", argc, argv, "a=15,o=1");
	int adj = H['a'];

	auto C = std::make_unique <Circle> (0,.5); cpx & z = C->z;
	auto P = std::make_unique <Path> (vector<cpx> {0}); vector<cpx> & path = P->z;
	Figure f(H['o']); f.add (std::move(C)); f.add (std::move(P)); f.margin = 1; f.show();

	for (int a=1; a<200; ++a) {
		for (int b=0; b<a; ++b) {
			int d=9; while (!((1<<d) & adj)) d = prng.uniform_int(8);
			z += cpx(dz[d]); path.push_back(z);
		}
		f.update();
	}
}
