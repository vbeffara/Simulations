/*
 * Simulation d'une marche simple dans Z^2, tuée à la sortie d'un
 * carré de côté donné. 
 */

#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/Figure.h>

using namespace vb;
using namespace std;

int dx[4] = {1,0,-1,0};  /* 0=est, 1=nord ... */
int dy[4] = {0,1,0,-1};

typedef pair<cpx,cpx> bicpx;

int main(int argc, char ** argv) {
  Figure f;
  vector < bicpx > t;
  PRNG prng(0);
  
  int x=0, y=0;
  int xmax=0, ymax=0;

  f.circle(0,.5);  
  f.show();
  f.pause();

  for (int a=1 ;; ++a) {
    for (int b=0; b<a; ++b) {
      int d = prng.uniform_int(4);
      int nx = x + dx[d]; if ((nx>xmax) || (-nx>xmax)) ++xmax;
      int ny = y + dy[d]; if ((ny>ymax) || (-ny>ymax)) ++ymax;
      pair<cpx,cpx> step (cpx(x,y),cpx(nx,ny));
      bool there=false;
      foreach (bicpx s, t) if (s==step) { there=true; break; }
      if (!there) t.push_back(step);
      x = nx; y = ny;
    }
    
    f.clean();
    f.circle(cpx(x,y),.5);
    f.dot(cpx(xmax+1,ymax+1));
    f.dot(cpx(xmax+1,-ymax-1));
    f.dot(cpx(-xmax-1,ymax+1));
    f.dot(cpx(-xmax-1,-ymax-1));
    for (unsigned i=0; i<t.size(); ++i)
      f.segment(t[i].first,t[i].second);
    f.show();
    f.snapshot();
  }
}
