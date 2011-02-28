/*
 * Simulation d'une marche simple dans Z^2, tuée à la sortie d'un
 * carré de côté donné. 
 */

#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/Figure.h>

using namespace vb;

int dx[8] = {1,1,0,-1,-1,-1,0,1};
int dy[8] = {0,1,1,1,0,-1,-1,-1};

typedef std::pair<cpx,cpx> bicpx;

int main(int argc, char ** argv) {
  CL_Parser CLP (argc, argv, "a=85,c,v");
  int  adj    = CLP('a');
  bool center = CLP('c');
  bool video  = CLP('v');
  
  Figure f;
  std::vector < bicpx > t;
  PRNG prng(0);
  
  int x=0, y=0;
  int xmax=0, ymax=0, xmin=0, ymin=0;

  f.circle(0,.5);  
  f.show();
  f.pause();

  for (int a=1 ;; ++a) {
    for (int b=0; b<a; ++b) {
      int d = 9; while (((1<<d) & adj) == 0) d = prng.uniform_int(8);
      
      int nx = x + dx[d]; if (nx>xmax) ++xmax; if (nx<xmin) --xmin;
      int ny = y + dy[d]; if (ny>ymax) ++ymax; if (ny<ymin) --ymin;
      
      std::pair<cpx,cpx> step (cpx(x,y),cpx(nx,ny));
      
      bool there=false;
      foreach (bicpx s, t) if (s==step) { there=true; break; }
      if (!there) t.push_back(step);
      
      x = nx; y = ny;
    }

    if (center) {
      xmax = max(xmax,-xmin); xmin = -xmax;
      ymax = max(ymax,-ymin); ymin = -ymax;
    }
    
    f.clean();
    f.circle(cpx(x,y),.5);
    f.dot(cpx(xmax+1,ymax+1));
    f.dot(cpx(xmax+1,ymin-1));
    f.dot(cpx(xmin-1,ymax+1));
    f.dot(cpx(xmin-1,ymin-1));
    for (unsigned i=0; i<t.size(); ++i)
      f.segment(t[i].first,t[i].second);
    f.show();
    if (video) f.snapshot();
  }
}
