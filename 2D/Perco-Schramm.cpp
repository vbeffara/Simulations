
/** Exploration of hexagon percolation in the half-plane.
 *
 * Initially written in postscript by Oded Schramm, and ported to C++
 * for convenience and speed.
 */

#include <cmath>
#include <iostream>

using namespace std;

////////////////////////////////////////////////////////////////
//
// Poor man's PS emulation, good enough here.

double x=0, y=0;

void moveto (double xx, double yy) {
  cout << "moveto (" << xx << "," << yy << ")" << endl;
  x=xx; y=yy;
}

void lineto (double xx, double yy) {
  cout << "lineto (" << x << "," << y << ") -> ("<< xx << "," << yy << ")" << endl;
  x=xx; y=yy;
}

void rmoveto (double xx, double yy) { moveto (x+xx, y+yy); }
void rlineto (double xx, double yy) { lineto (x+xx, y+yy); }

//
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
  moveto (xmin,ymin);
  lineto (xmin,ymax);
  lineto (xmax,ymax);
  lineto (xmax,ymin);
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

void hex (double d0, double d1) {
  moveto (d1,d0); // swapped between moveto and the others !
  rmoveto (-omx,-.5);
  rlineto (0,1);
  rlineto (omx,.5);
  rlineto (omx,-.5);
  rlineto (0,-1);
  rlineto (-omx,-.5);
  rlineto (-omx,.5);
}

// /setbits
//   {/cols [ numw 2 idiv { true} repeat
// 	   numw 2 idiv {false} repeat
// 	   numw 1 sub numh mul  {rand rand lt} repeat ] def} def

// /thebit { cols exch get } def

// /thepos {dup dup numw mod 2 mul exch rowparity  add omx mul 
//          exch numw idiv  3 2 div mul } def

// /rowparity { numw idiv 2 mod } def

// /drawhexes { 0 1 numw numh mul 1 sub  {thepos hex stroke } for} def

// /perc { 0 1 numw numh mul 1 sub 
// 	{dup thebit { thepos hex fill} {pop} ifelse  } for} def

// /rotleft {1 add 6 mod} def

// /rotright {5 add 6 mod} def

// /thenext { base dir rotleft follow } def

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

// /spillout { base 1 add numw mod 2 lt 
//             base numw div numh 2 sub gt
// 	    or } def

// /fola [ 1 numw numw 1 sub 1 neg numw neg 1 sub numw neg  ] def

// /folb [ 1   numw 1 add    numw  -1  numw neg numw neg 1 add ] def

// /follow { /dirf exch def /basef exch def
//          base rowparity 0 eq { fola } { folb } ifelse dirf get basef add} def

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
//
// 26 srand
// picture

int main (int argc, char ** argv) {
  return 0;
}
