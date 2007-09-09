/* Test for the vb::Figure class and associates. */

#include <iostream>
#include <vb/Figure.h>

using namespace std;
using namespace vb;

int main (int argc, char **argv) {
  Figure F;
  
  for (int i=0; i<=10; ++i) {
    F.add(new Segment (complex<double>(0,0.1*i), complex<double>(1,0.1*i)));
    F.add(new Segment (complex<double>(0.1*i,0), complex<double>(0.1*i,1)));
  }

  for (int i=1; i<=5; ++i)
    F.add(new Circle (complex<double>(.5,.5),.1*i));

  F.printASY(cout);
  return 0;
}
