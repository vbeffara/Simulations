
#include <vb/Console.h>
#include <vb/PRNG.h>

using namespace vb;

double x2 (void *x) {
  double X = *(double*)x; return X*2;
}

int main (int argc, char ** argv) {
  double x=1.1;

  Console W1;
  W1.watch (x,"x");
  W1.show();

  for (unsigned long i=0; i<3e8; ++i) {
    x = prng.uniform_real();
    global_clock.step();
  }

  W1.trace (x2,&x,"Double of x");

  for (unsigned long i=0; i<3e8; ++i) {
    x = prng.uniform_real();
    global_clock.step();
  }

  return 0;
}
