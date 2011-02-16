/*
 * Simulation d'une marche simple dans Z^2, tuée à la sortie d'un
 * carré de côté donné. 
 */

#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/Figure.h>

using namespace vb;

int dx[4] = {1,0,-1,0};  /* 0=est, 1=nord ... */
int dy[4] = {0,1,0,-1};

int main(int argc, char ** argv) {
  Figure f;
  std::vector<cpx> t(1);
  PRNG prng(0);
  
  int x=0, y=0;
  int xmax=0, ymax=0;

  f.circle(0,.5);  
  f.show();
  f.pause();

  for (int a=1 ;; ++a) {
    for (int b=0; b<a; ++b) {
      int d = prng.uniform_int(4);
      x += dx[d]; if ((x>xmax) || (-x>xmax)) ++xmax;
      y += dy[d]; if ((y>ymax) || (-y>ymax)) ++ymax;
      t.push_back(cpx(x,y));
    }
    
    f.clean();
    f.circle(cpx(x,y),.5);
    f.dot(cpx(xmax+1,ymax+1));
    f.dot(cpx(xmax+1,-ymax-1));
    f.dot(cpx(-xmax-1,ymax+1));
    f.dot(cpx(-xmax-1,-ymax-1));
    for (unsigned i=0; i<t.size()-1; ++i)
      f.segment(t[i],t[i+1]);
    f.show();
  }
}
