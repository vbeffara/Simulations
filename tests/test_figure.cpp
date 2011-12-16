
/// @file figtest.cpp
/// A simple example of how to use the vb::Figure class.

#include <vb/Figure.h>
#include <vb/PRNG.h>

using namespace std;
using namespace vb;

Color randomcolor() {
  return Color (prng.uniform_int(256),
                prng.uniform_int(256),
                prng.uniform_int(256));
}

/// The main function.
int main (int argc, char **argv) {
  prng.seed(18);

  Figure F;
  for (int i=0; i<10; ++i) {
    F.add (new Segment (cpx(prng.uniform_real(-10,10),prng.uniform_real(-10,10)),
                        cpx(prng.uniform_real(-10,10),prng.uniform_real(-10,10)),
                        randomcolor()));
  }

  for (int i=1; i<=5; ++i)
    F.add (new Circle (cpx(0,0), i));

  F.show(); F.pause();

  F.output ("output/test_figure");
  exit (0);
}
