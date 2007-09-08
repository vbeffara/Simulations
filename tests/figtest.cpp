/* Test for the vb::Figure class and associates. */

#include <iostream>
#include <vb/Figure.h>

using namespace std;
using namespace vb;

int main (int argc, char **argv) {
  Figure F;
  
  F.add(new Segment (complex<double>(0,0), complex<double>(2,1)));

  F.printASY(cout);
  return 0;
}
