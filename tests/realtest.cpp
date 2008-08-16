
#include <vb/Real.h>
#include <vb/PRNG.h>
#include <iostream>

using namespace std;
using namespace vb;

int main () {
  PRNG prng;
  Real r = prng.uniform();
  Real sr = sin(r);

#ifdef HAVE_CLN
  cout << "Using the CLN library." << endl;
#else
  cout << "Using plain old double." << endl;
#endif

  cout << r << endl << sr << endl;
}
