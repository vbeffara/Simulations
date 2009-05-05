/*
 * Directed DLA
 */

#include <vb.h>
#include <vb/Point.h>
#include <iostream>
#include <cmath>

using namespace vb;

CoarseImage *img;
int cursum;
int n,maxx,maxy;
double p;
bool s;

PRNG prng;

void addapoint (int x, int y) {
  img->putpoint(x,y,1);
  if ( (x>0) && (y<n-1) && ((*img)(x-1,y+1)==1) && ((*img)(x-1,y)==0) )
    addapoint (x-1,y);
  if ( (y>0) && (x<n-1) && ((*img)(x+1,y-1)==1) && ((*img)(x,y-1)==0) )
    addapoint (x,y-1);
}

void reshape (int x, int y) {
  if (x==maxx) ++maxx;
  if (y==maxy) ++maxy;
  if (x+y>cursum) cursum=x+y;
}

bool ddla (int x, int y) {
  int xx=x, yy=y;
  bool just_started=true;

  while (1) {
    if (prng.uniform()<p) ++xx;
    else ++yy;
    if ((xx>=n) || (yy>=n) || (xx+yy > cursum)) return 1;
    if ((*img)(xx,yy) == 1) return 0;
    if (s && (!just_started) && (xx<n-1) && (yy<n-1) &&
        (((*img)(xx+1,yy)==1) || ((*img)(xx,yy+1)==1))) return 0;
    just_started = false;
  }
}

bool corner_plus_top (int x, int y) {
  if ( (y==maxy) || (x==maxx) ) return true;
  int bot   = (y==0  ?0:(*img)(x,y-1));
  int top   = (y==n-1?0:(*img)(x,y+1));
  int left  = (x==0  ?0:(*img)(x-1,y));
  int right = (x==n-1?0:(*img)(x+1,y));

  if (bot+top+left+right==2) return true;
  else return false;
}

bool ok (int x, int y) {
  return ddla(x,y);
  return corner_plus_top(x,y);
}

int main (int argc, char **argv) {
  CL_Parser CLP (argc,argv,"n=2000,p=.5,s");
  n = CLP('n');
  p = CLP('p');
  s = CLP('s');

  PointQueue queue;

  maxx=1; maxy=1; cursum=0;

  img = new CoarseImage (n,n,(int)pow((double)n,.33),"A directed DLA cluster");
  img->show();

  img->putpoint(0,0,1);
  queue << Point(1,0,prng.exponential())
        << Point(0,1,prng.exponential());

  int running=1;
  while (running) {
    Point pt (queue);
    
    if ((pt.x==n-1) || (pt.y==n-1)) running=0;
    else if ((*img)(pt.x,pt.y)==0) {
      double curtime = pt.t;
      if (ok(pt.x,pt.y)) {
	addapoint (pt.x,pt.y);
	reshape (pt.x,pt.y);
	if ( (pt.x<n-1) && ((*img)(pt.x+1,pt.y)==0) )
	  queue << Point(pt.x+1,pt.y,curtime+prng.exponential()/p);
	if ( (pt.y<n-1) && ((*img)(pt.x,pt.y+1)==0) )
	  queue << Point(pt.x,pt.y+1,curtime+prng.exponential()/(1-p));
        if ( s && (pt.x>0) && ((*img)(pt.x-1,pt.y)==0) )
          queue << Point(pt.x-1,pt.y,curtime+prng.exponential());
        if ( s && (pt.y>0) && ((*img)(pt.x,pt.y-1)==0) )
          queue << Point(pt.x,pt.y-1,curtime+prng.exponential());
      } else {
	queue << Point(pt.x,pt.y,curtime+prng.exponential());
      }
    }
  }

  std::cout << *img;

  return 0;
}
