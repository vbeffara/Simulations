/* Test for the vb::Figure class and associates. */

#include <iostream>
#include <vb/Figure.h>

using namespace std;
using namespace vb;

int main (int argc, char **argv) {
  Figure F;
  
  for (int i=0; i<=10; ++i) {
    F.segment (cpx(0,0.1*i), cpx(1,0.1*i))
     .dot (cpx(.1*i,.1*i))
     .segment (cpx(0.1*i,0), cpx(0.1*i,1))
     .dot (cpx(.1*i,.1*i));
  }

  for (int i=1; i<=5; ++i)
    F.circle (cpx(.5,.5),.1*i);

  F.printASY("toto.asy");
  F.unique();
  F.printASY("toto.clean.asy");
  return 0;
}
