
#include <vb/Watcher.h>
#include <vb/Clock.h>

using namespace vb;
using namespace std;

void hello (void * W) {
  cerr << *((Watcher*)W);
}

int main (int argc, char ** argv) {
  double x=1.1;

  Watcher W;
  W.add (new Value<double> (x,"x"));

  Clock C;
  C.add (100,hello,&W);

  while (true) {
    x = rand();
    C.run();
  }

  return 0;
}
