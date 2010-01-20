
/// @file figtest.cpp
/// A simple example of how to use the vb::Figure class.

#include <vb/Figure.h>
#include <vb/PRNG.h>

using namespace std;
using namespace vb;

/// The main function.
int main (int argc, char **argv) {
  Figure F;
  PRNG prng (18);
  
  for (int i=0; i<10; ++i) {
    F.segment (cpx(-10+prng.uniform(20.0),-10+prng.uniform(20.0)),
               cpx(-10+prng.uniform(20.0),-10+prng.uniform(20.0)));
  }

  for (int i=1; i<=5; ++i)
    F.circle (cpx(0,0),i);

  //F.show();

  F.output ("output/test_figure");
  exit (0);
}
