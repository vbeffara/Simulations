
#include <vb/Watcher.h>

using namespace vb;
using namespace std;

void hello (void * W) {
  cerr << *((Watcher*)W);
}

int main (int argc, char ** argv) {
  double x=1.1;

  Watcher W1;
  W1.add (new Value<double> (x,"x"));

  {
    Watcher W2;
    W2.add (new Value<double> (x,"xx"));

    for (unsigned long i=0; i<1e9; ++i) {
      x = rand();
      global_clock.step();
    }
  }

  cerr << "Now W2 does not exist anymore ..." << endl;

  for (unsigned long i=0; i<1e9; ++i) {
    x = rand();
    global_clock.step();
  }

  return 0;
}
