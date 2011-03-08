
/** Exploration of hexagon percolation in the half-plane.
 *
 * Initially written in postscript by Oded Schramm, and ported to C++
 * for convenience.
 */

#include <vb/Figure.h>

using namespace std;
using namespace vb;

double omx = sqrt(3.0);

class Perco_Schramm : public Figure {
public:
  int w, h, base, dir;
  vector<bool> cols;
  cpx pos;
  
  Perco_Schramm (int w_, int h_) : w(w_), h(h_) {
    for (int i=0; i < w/2; ++i)     cols.push_back (true);
    for (int i=0; i < w/2; ++i)     cols.push_back (false);
    for (int i=0; i < (w-1)*h; ++i) cols.push_back (rand()<rand());
  }
  
  void lineto (cpx xy) {
    Figure::segment (pos,xy);
    pos = xy;
  }
  
  void rlineto (cpx xy) { lineto (pos+xy); }

  void hex (cpx xy) {
    pos = xy + cpx(-omx,-1);
    rlineto (cpx(0,2));  rlineto (cpx(omx,1));   rlineto (cpx(omx,-1));
    rlineto (cpx(0,-2)); rlineto (cpx(-omx,-1)); rlineto (cpx(-omx,1));
  }
  
  cpx thepos (int i) {
    return cpx(omx*(((i/w)%2)+2*(i%w)) , 3*(i/w));
  }
  
  void perc () {
    for (int i=0; i<w*h; ++i) if (cols[i]) hex(thepos(i));
  }
  
  int follow (int dir, int base) {
    int fola[6] = { 1, w, w-1, -1, -w-1, -w };
    int folb[6] = { 1, w+1, w, -1, -w, -w+1 };
    return ((base/w)%2 ? folb : fola) [dir] + base;
  }
  
  int thenext() { return follow ((dir+1)%6, base); }
  
  void segment (int rot) {
    cpx x1y1 = thepos(base);
    cpx x2y2 = thepos(follow(dir,base));
    cpx x3y3 = thepos(follow((dir+rot)%6,base));
    pos = (x1y1+x2y2) * .5;
    lineto ((x1y1+x2y2+x3y3) * (1.0/3));
  }

  void walk () {
    base = w/2-1;
    dir = 0;
    while (((base+1)%w >= 2) && (base/w <= h-2)) {
      segment (1);
      if (cols[thenext()]) { base = thenext(); dir = (dir+5)%6; }
      else                 {                   dir = (dir+1)%6; }
      segment (5);
    }
  }  
};

int main (int argc, char ** argv) {
  Perco_Schramm RS(60,70);
  RS.walk();
  RS.show();
  RS.pause();
  return 0;
}
