
#include <vb/Console.h>
#include <vb/PRNG.h>

using namespace vb;

double x2 (void *x) { double X = *(double*)x; return X*2; }

int main (int, char **) {
	double x=1.1;

	Console W;
	W.watch (x,"x");
	W.show();

	for (unsigned long i=0; i<3e8; ++i) { x = prng.uniform_real(); W.step(); } // Usually W.step() would be implicit

	W.trace (x2,&x,"Double of x");

	for (unsigned long i=0; i<3e8; ++i) { x = prng.uniform_real(); W.step(); }
}
