
/// @file figtest.cpp
/// A simple example of how to use the vb::Figure class.

#include <iostream>
#include <vb/Figure.h>
#include <vb/PRNG.h>
#include <fltk/run.h>

using namespace std;
using namespace vb;

/// The main function.
int main (int argc, char **argv) {
  Figure F;
  PRNG prng;
  
  for (int i=0; i<10; ++i) {
    F.segment (cpx(-10+prng.uniform(20.0),-10+prng.uniform(20.0)),
               cpx(-10+prng.uniform(20.0),-10+prng.uniform(20.0)));
  }

  for (int i=1; i<=5; ++i)
    F.circle (cpx(0,0),i);

#ifdef LIBVB_FLTK
  F.resizable(&F);
  F.show();
  fltk::run();
#endif
  exit (0);
}
