
/** Exploration of hexagon percolation in the half-plane.
 *
 * Initially written in postscript by Oded Schramm, and ported to C++
 * for convenience.
 */

#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

typedef pair<double,double> coo;

coo operator+ (coo a, coo b)    { return coo (a.first+b.first , a.second+b.second); }
coo operator/ (coo a, double l) { return coo (a.first/l       , a.second/l       ); }

ostream &operator<< (ostream &o, coo c) {
  return o << "(" << c.first << "," << c.second << ")";
}

double omx = sqrt(3.0);

class Perco_Schramm {
public:
  int w, h, base, dir;
  vector<bool> cols;
  coo pos;
  
  Perco_Schramm (int w_, int h_) : w(w_), h(h_) {
    for (int i=0; i < w/2; ++i)     cols.push_back(true);
    for (int i=0; i < w/2; ++i)     cols.push_back(false);
    for (int i=0; i < (w-1)*h; ++i) cols.push_back(rand()<rand());
  }
  
  void lineto (coo xy) {
    cout << "draw(" << pos << "--"<< xy << ");" << endl;
    pos = xy;
  }
  
  void rlineto (coo xy) { lineto (pos+xy); }

  void hex (coo xy) {
    pos = xy + coo(-omx,-1);
    rlineto (coo(0,2));  rlineto (coo(omx,1));   rlineto (coo(omx,-1));
    rlineto (coo(0,-2)); rlineto (coo(-omx,-1)); rlineto (coo(-omx,1));
  }
  
  coo thepos (int i) {
    return coo(omx*(((i/w)%2)+2*(i%w)) , 3*(i/w));
  }
  
  void perc () {
    for (int i=0; i<w*h; ++i) if (cols[i]) hex(thepos(i));
  }
  
  int follow (int dir, int base) {
    int fola[6] = { 1, w, w-1, -1, -w-1, -w };
    int folb[6] = { 1, w+1, w, -1, -w, -w+1 };
    return ((base/w)%2 == 0 ? fola : folb) [dir] + base;
  }
  
  int thenext() { return follow ((dir+1)%6, base); }
  
  void segment (int rot) {
    coo x1y1 = thepos(base);
    coo x2y2 = thepos(follow(dir,base));
    coo x3y3 = thepos(follow((dir+rot)%6,base));
    pos = (x1y1+x2y2)/2;
    lineto ((x1y1+x2y2+x3y3)/3);
  }

  void walk () {
    base = w/2-1;
    dir = 0;
    bool basebit = cols[base];
    while (((base+1)%w >= 2) && (base/w <= h-2)) {
      segment(1);
      if (cols[thenext()]==basebit) {
        base = thenext();
        dir = (dir+5)%6;
      } else {
        dir = (dir+1)%6;
      }
      segment(5);
    }
  }  
};

int main (int argc, char ** argv) {
  Perco_Schramm(40,40).walk();
  return 0;
}
