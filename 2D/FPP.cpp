/*
 * FPP.c - First Passage Percolation
 * TODO switch to coo everywhere
 */

#include <vb/Point.h>

using namespace vb;

double cost () { return prng.exponential(); }

class FPP : public CoarseImage {
public:
  FPP (int n) : CoarseImage (n,n, pow(n,.33)), area(0) {
    invasion = H['i'];
    twostep  = H['2'];
    trace    = H['t'];

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
    while (true) {
      if (trace) std::cout << area << " " << pqueue.size() << std::endl;

      Point pt(pqueue);

      if (!at(pt)) {
        (*this) << pt; ++area;

        double curtime = invasion ? 0.0 : pt.t;

        int deg=1;
        if (twostep) deg = at(pt-coo(1,0)) + at(pt+coo(1,0)) + at(pt+coo(0,1)) + at(pt-coo(0,1));
        for (int i=0; i<deg; ++i) spread (curtime,pt.x,pt.y);

        if (twostep) {
          if (at(pt+coo(-1,0))) spread(curtime,pt.x-1,pt.y);
          if (at(pt+coo(+1,0))) spread(curtime,pt.x+1,pt.y);
          if (at(pt+coo(0,-1))) spread(curtime,pt.x,pt.y-1);
          if (at(pt+coo(0,+1))) spread(curtime,pt.x,pt.y+1);
        }

        if ( (pt.x==1) || (pt.y==1) || (pt.x==true_width-2) || (pt.y==true_height-2) ) break;
      }
    }
  }

  int area;
  bool trace, invasion, twostep;

  PointQueue pqueue;
};

int main (int argc, char **argv) {
  H.init ("First-passage percolation", argc,argv, "n=5000,i,2,t");

  FPP F (H['n']);
  F.show();
  F.run();
  F.output();

  return 0;
}
