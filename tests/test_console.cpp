
#include <vb/Console.h>
#include <vb/PRNG.h>

using namespace vb;

int main (int, char **) {
	double x=1.1;

	Console W;
	W.watch (x,"x");
	W.show();

	for (unsigned long i=0; i<1e8; ++i) { x = prng.uniform_real(); W.step(); } // Usually W.step() would be implicit

	W.lambda<double> ([&x]{return 2*x;}, "Double of x");

	for (unsigned long i=0; i<1e8; ++i) { x = prng.uniform_real(); W.step(); }
}
