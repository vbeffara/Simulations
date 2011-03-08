
/** Exploration of hexagon percolation in the half-plane.
 *
 * Initially written in postscript by Oded Schramm, and ported to C++
 * for convenience and speed.
 */

#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

////////////////////////////////////////////////////////////////
//
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

void moveto (coo xy) {
  pos = xy;
}

void lineto (coo xy) {
  cout << "draw(" << pos << "--"<< xy << ");" << endl;
  pos = xy;
}

void rmoveto (coo xy) { moveto (pos+xy); }
void rlineto (coo xy) { lineto (pos+xy); }

////////////////////////////////////////////////////////////////

double xmin = 60;
double xmax = 500;
double ymin = 350;
double ymax = 640;

int numw = 100;                 // Must be even

double marginhexes = 1.2;

double picwidth = xmax-xmin;

double eachhex = picwidth / (numw-(marginhexes*2)-1); // The width of each hexagon

double omx = sqrt(3.0)/2;

int numh = (ymax-ymin)/(eachhex*omx) + marginhexes;

// /outbox { xmin ymin xmax ymax box } def

void outbox () {
  moveto (coo(xmin,ymin));
  lineto (coo(xmin,ymax));
  lineto (coo(xmax,ymax));
  lineto (coo(xmax,ymin));
}

// Setgraphics : purely PS stuff, see later, covered by libvb.
//
// /setgraphics {
//   xmin marginhexes eachhex mul sub
//   ymin eachhex add 
//   translate
//   eachhex omx div 2 div dup 
//   scale
//   1 setlinecap
//   0.00001 setlinewidth} def

// Hex : draw a hexagon locally at the given coordinates.
//
// /hex {newpath moveto
//   omx neg 0.5 neg rmoveto
//   0 1 rlineto
//   omx 0.5 rlineto
//   omx 0.5 neg rlineto
//   0 1 neg rlineto
//   omx neg 0.5 neg rlineto
//   closepath} bind def

void hex (coo xy) {
  moveto (xy);
  rmoveto (coo(-omx,-.5));
  rlineto (coo(0,1));
  rlineto (coo(omx,.5));
  rlineto (coo(omx,-.5));
  rlineto (coo(0,-1));
  rlineto (coo(-omx,-.5));
  rlineto (coo(-omx,.5));
}

// /setbits
//   {/cols [ numw 2 idiv { true} repeat
// 	   numw 2 idiv {false} repeat
// 	   numw 1 sub numh mul  {rand rand lt} repeat ] def} def

vector<bool> cols;

void setbits () {
  cols.clear();
  for (int i=0; i < numw/2; ++i)        cols.push_back(true);
  for (int i=0; i < numw/2; ++i)        cols.push_back(false);
  for (int i=0; i < (numw-1)*numh; ++i) cols.push_back(rand()<rand());  
}

// /thebit { cols exch get } def

bool thebit (int i) { return cols[i]; }

// /rowparity { numw idiv 2 mod } def

int rowparity (int i) { return (i/numw)%2; }

// /thepos {dup dup numw mod 2 mul exch rowparity  add omx mul 
//          exch numw idiv  3 2 div mul } def

coo thepos (int i) {
  return coo(omx*(((i/numw)%2)+2*(i%numw)) , 1.5*(i/numw));
}

// /drawhexes { 0 1 numw numh mul 1 sub  {thepos hex stroke } for} def

void drawhexes () {
  for (int i=0; i<numw*numh; ++i) {
    hex(thepos(i));
  }
}

// /perc { 0 1 numw numh mul 1 sub 
// 	{dup thebit { thepos hex fill} {pop} ifelse  } for} def

void perc () {
  for (int i=0; i<numw*numh; ++i) {
    if (thebit(i)) hex(thepos(i));
  }
}

// /rotleft {1 add 6 mod} def

int rotleft (int d) { return (d+1)%6; }

// /rotright {5 add 6 mod} def

int rotright (int d) { return (d+5)%6; }

// /spillout { base 1 add numw mod 2 lt 
//             base numw div numh 2 sub gt
// 	    or } def

int base;

bool spillout () {
  return (((base+1)%numw)<2) || ((base/numw)>(numh-2));
}

// /fola [ 1  numw  numw 1 sub  1 neg  numw neg 1 sub  numw neg ] def

// /folb [ 1  numw 1 add  numw  -1  numw neg  numw neg 1 add ] def

// /follow { /dirf exch def /basef exch def
//          base rowparity 0 eq { fola } { folb } ifelse dirf get basef add} def

int follow (int dirf, int basef) {
  int fola[6] = { 1, numw, numw-1, -1, -numw-1, -numw };
  int folb[6] = { 1, numw+1, numw, -1, -numw, -numw+1 };
  return (rowparity(base)==0 ? fola : folb) [dirf] + basef;
}

// /thenext { base dir rotleft follow } def

int dir;

int thenext() {
  return follow (rotleft(dir), base);
}

// /segment {/therot exch def
// 	base thepos /y1 exch def /x1 exch def
//         base dir follow thepos /y2 exch def /x2 exch def
//         base dir therot follow thepos /y3 exch def /x3 exch def
// 	newpath 
// 	x1 x2 add 2 div y1 y2 add 2 div 
// 	moveto
// 	x1 x2 x3 add add 3 div
// 	y1 y2 y3 add add 3 div 
// 	lineto
// 	stroke } def

void segment (int therot(int)) {
  coo x1y1 = thepos(base);
  coo x2y2 = thepos(follow(dir,base));
  coo x3y3 = thepos(follow(therot(dir),base));
  moveto ((x1y1+x2y2)/2);
  lineto ((x1y1+x2y2+x3y3)/3);
}

// /walk { /base numw 2 idiv 1 sub def
//   	/dir 0 def
//   	/basebit base thebit def
// 	{
// 		spillout {exit} if
// 		{rotleft} segment
// 	  	thenext thebit basebit eq
// 		{ /base thenext def
// 		  /dir dir rotright def }
// 		{ /dir dir rotleft def }
// 		ifelse
// 		{rotright} segment
// 	} loop
//   } def

void walk () {
  base = numw/2-1;
  dir = 0;
  bool basebit = thebit(base);
  while (!spillout()) {
    segment(rotleft);
    if (thebit(thenext())==basebit) {
      base = thenext();
      dir = rotright(dir);
    } else {
      dir = rotleft(dir);
    }
    segment(rotright);
  }
}

// /picture 
//   {
//   outbox clip 
//   1 setgray 
//   outbox fill
//   0 setgray 
//   setgraphics 
//   setbits
//   0.7 setgray
//   perc
//   0 setgray
//   drawhexes
//   0.5 setlinewidth
//   walk
//   showpage
//   } def

void picture () {
  setbits();
  //  perc();
  //  drawhexes();
  walk();
}

//
// 26 srand
// picture

int main (int argc, char ** argv) {
  picture();
  return 0;
}
