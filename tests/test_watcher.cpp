
#include <vb/Watcher.h>

using namespace vb;
using namespace std;

void hello (void * W) {
  cerr << *((Watcher*)W);
}

double x2 (void *x) {
  double X = *(double*)x; return X*2;
}

int main (int argc, char ** argv) {
  double x=1.1;

  Watcher W1;
  W1.watch (new Value<double> (x,"x"));
  W1.watch (new Value_calc<double> (x2,&x,"Double of x"));
  W1.show();

  for (unsigned long i=0; i<3e8; ++i) {
    x = rand();
    global_clock.step();
  }

  {
    Watcher W2;
    W2.watch (new Value<double> (x,"xx"));
    W2.show();

    for (unsigned long i=0; i<3e8; ++i) {
      x = rand();
      global_clock.step();
    }
  }

  for (unsigned long i=0; i<3e8; ++i) {
    x = rand();
    global_clock.step();
  }

  return 0;
}
