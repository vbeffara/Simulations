/*
 * FPP.c - First Passage Percolation
 */

#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/Point.h>

using namespace vb;

double cost (void) {
  return prng.exponential();
}

class FPP : public CoarseImage {
public:
  FPP (int n, CL_Parser &CLP) : CoarseImage (n,n, "First-Passage Percolation", pow(n,.33)), area(0) {
    invasion = CLP('i');
    twostep  = CLP('2');
    trace    = CLP('t');

    pqueue << Point(coo(n>>1,n>>1),cost());
    if (twostep) pqueue << Point (coo((true_width>>1)+1, true_height>>1), cost());
  };

  void spread (double t, int x, int y) {
    if (!(at(x+1,y))) pqueue << Point(coo(x+1,y),t+cost());
    if (!(at(x-1,y))) pqueue << Point(coo(x-1,y),t+cost());
    if (!(at(x,y+1))) pqueue << Point(coo(x,y+1),t+cost());
    if (!(at(x,y-1))) pqueue << Point(coo(x,y-1),t+cost());
  }

  void run () {
    while (1) {
      if (trace) std::cout << area << " " << pqueue.size() << std::endl;

      Point pt(pqueue);

      if (!(at(real(pt.z),imag(pt.z)))) {
        (*this) << pt; ++area;

        double curtime = invasion ? 0.0 : pt.t;

        int deg=1;
        if (twostep) deg = at(real(pt.z)-1,imag(pt.z)) + at(real(pt.z)+1,imag(pt.z)) + at(real(pt.z),imag(pt.z)+1) + at(real(pt.z),imag(pt.z)-1);
        for (int i=0; i<deg; ++i) spread (curtime,real(pt.z),imag(pt.z));

        if (twostep) {
          if (at(real(pt.z)-1,imag(pt.z))) spread(curtime,real(pt.z)-1,imag(pt.z));
          if (at(real(pt.z)+1,imag(pt.z))) spread(curtime,real(pt.z)+1,imag(pt.z));
          if (at(real(pt.z),imag(pt.z)-1)) spread(curtime,real(pt.z),imag(pt.z)-1);
          if (at(real(pt.z),imag(pt.z)+1)) spread(curtime,real(pt.z),imag(pt.z)+1);
        }

        if ( (real(pt.z)==1) || (imag(pt.z)==1) || (real(pt.z)==true_width-2) || (imag(pt.z)==true_height-2) ) break;
      }
    }
  }

  int area;
  bool trace, invasion, twostep;

  PointQueue pqueue;
};

int main (int argc, char **argv) {
  CL_Parser CLP (argc,argv,"n=5000,i,2,t");

  FPP F (CLP('n'), CLP);
  F.show();
  F.run();
  F.output();

  return 0;
}
