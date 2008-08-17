#include <iostream>
#include <vb/Timer.h>

using namespace vb;
using namespace std;

int main () {
  Timer t1 (5000);

  for (int i=0; i<100; ++i) {
    if (t1.check()) {
      cout << "========" << endl;
      t1.reset();
    }
    cout << t1.sec << ":" << t1.usec << endl;
  }
}
