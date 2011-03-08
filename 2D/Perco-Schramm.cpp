
/** Exploration of hexagon percolation in the half-plane.
 *
 * Initially written in postscript by Oded Schramm, and ported to C++
 * for convenience.
 */

#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

// Poor man's PS emulation, good enough here.

typedef pair<double,double> coo;

coo operator+ (coo a, coo b) {
  return coo (a.first+b.first, a.second+b.second);
}

coo operator/ (coo a, double l) {
  return coo (a.first/l, a.second/l);
}

ostream &operator<< (ostream &o, coo c) {
  o << "(" << c.first << "," << c.second << ")";
  return o;
}

coo pos;

void lineto (coo xy) {
  cout << "draw(" << pos << "--"<< xy << ");" << endl;
  pos = xy;
}

void rlineto (coo xy) { lineto (pos+xy); }

// Parameters

int    numw        = 100;                                 // Must be even
double marginhexes = 1.2;
double picwidth    = 440;
double eachhex     = picwidth / (numw-(marginhexes*2)-1); // The width of each hexagon
// double picwidth    = eachhex * (numw-(marginhexes*2)-1); // The width of each hexagon
double omx         = sqrt(3.0)/2;
int    numh        = 290/(eachhex*omx) + marginhexes;

// Global variables

vector<bool> cols;

int base,dir;

void hex (coo xy) {
  pos = xy + coo(-omx,-.5);
  rlineto (coo(0,1));
  rlineto (coo(omx,.5));
  rlineto (coo(omx,-.5));
  rlineto (coo(0,-1));
  rlineto (coo(-omx,-.5));
  rlineto (coo(-omx,.5));
}

coo thepos (int i) {
  return coo(omx*(((i/numw)%2)+2*(i%numw)) , 1.5*(i/numw));
}

void drawhexes () {
  for (int i=0; i<numw*numh; ++i) {
    hex(thepos(i));
  }
}

void perc () {
  for (int i=0; i<numw*numh; ++i) {
    if (cols[i]) hex(thepos(i));
  }
}

int rotleft  (int d) { return (d+1)%6; }
int rotright (int d) { return (d+5)%6; }

bool spillout () {
  return (((base+1)%numw)<2) || ((base/numw)>(numh-2));
}

int follow (int dir, int base) {
  int fola[6] = { 1, numw, numw-1, -1, -numw-1, -numw };
  int folb[6] = { 1, numw+1, numw, -1, -numw, -numw+1 };
  return ((base/numw)%2 == 0 ? fola : folb) [dir] + base;
}

int thenext() {
  return follow (rotleft(dir), base);
}

void segment (int therot(int)) {
  coo x1y1 = thepos(base);
  coo x2y2 = thepos(follow(dir,base));
  coo x3y3 = thepos(follow(therot(dir),base));
  pos = (x1y1+x2y2)/2;
  lineto ((x1y1+x2y2+x3y3)/3);
}

void walk () {
  base = numw/2-1;
  dir = 0;
  bool basebit = cols[base];
  while (!spillout()) {
    segment(rotleft);
    if (cols[thenext()]==basebit) {
      base = thenext();
      dir = rotright(dir);
    } else {
      dir = rotleft(dir);
    }
    segment(rotright);
  }
}

int main (int argc, char ** argv) {
  cerr << eachhex << endl;
  for (int i=0; i < numw/2; ++i)        cols.push_back(true);
  for (int i=0; i < numw/2; ++i)        cols.push_back(false);
  for (int i=0; i < (numw-1)*numh; ++i) cols.push_back(rand()<rand());
  // perc();
  // drawhexes();
  walk();
  return 0;
}
