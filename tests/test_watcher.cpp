
#include <vb/Watcher.h>

using namespace vb;
using namespace std;

int main (int argc, char ** argv) {
  double x=1.1;
  int i=12;
  Watcher W;

  W.add (new Value<double> (x,"x"));
  W.add (new Value<int> (i,"i"));

  cerr << W;
  x = 2.5;
  cerr << W;
  i = 234;
  cerr << W;

  return 0;
}
