/*
 * FPP.c - First Passage Percolation
 * TODO switch to coo everywhere
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
    if (!(at(coo(x+1,y)))) pqueue << Point(coo(x+1,y),t+cost());
    if (!(at(coo(x-1,y)))) pqueue << Point(coo(x-1,y),t+cost());
    if (!(at(coo(x,y+1)))) pqueue << Point(coo(x,y+1),t+cost());
    if (!(at(coo(x,y-1)))) pqueue << Point(coo(x,y-1),t+cost());
  }

  void run () {
    while (1) {
      if (trace) std::cout << area << " " << pqueue.size() << std::endl;

      Point pt(pqueue);

      if (!at(pt.z)) {
        (*this) << pt; ++area;

        double curtime = invasion ? 0.0 : pt.t;

        int deg=1;
        if (twostep) deg = at(pt.z-coo(1,0)) + at(pt.z+coo(1,0)) + at(pt.z+coo(0,1)) + at(pt.z-coo(0,1));
        for (int i=0; i<deg; ++i) spread (curtime,pt.z.x,pt.z.y);

        if (twostep) {
          if (at(pt.z+coo(-1,0))) spread(curtime,pt.z.x-1,pt.z.y);
          if (at(pt.z+coo(+1,0))) spread(curtime,pt.z.x+1,pt.z.y);
          if (at(pt.z+coo(0,-1))) spread(curtime,pt.z.x,pt.z.y-1);
          if (at(pt.z+coo(0,+1))) spread(curtime,pt.z.x,pt.z.y+1);
        }

        if ( (pt.z.x==1) || (pt.z.y==1) || (pt.z.x==true_width-2) || (pt.z.y==true_height-2) ) break;
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
