
#include <vb/Watcher.h>
#include <vb/Clock.h>

using namespace vb;
using namespace std;

void hello (void * W) {
  cerr << *((Watcher*)W);
}

int main (int argc, char ** argv) {
  double x=1.1;

  Watcher W1;
  W1.add (new Value<double> (x,"x"));

  Watcher W2;
  W2.add (new Value<double> (x,"xx"));

  Clock C;
  C.add (10,hello,&W1);
  C.add (100,hello,&W2);

  while (true) {
    x = rand();
    C.step();
  }

  return 0;
}
