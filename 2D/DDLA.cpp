/*
 * Directed DLA
 */

#include <vb.h>
#include <iostream>
#include <time.h>
#include <cmath>

using namespace vb;

CoarseImage *img;
int *shape;
int n,maxx,maxy;

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
  while ((x>=0) && (y>shape[x])) {
    shape[x]=y;
    --x;
  }
}

bool ddla (int x, int y) {
  int xx=x, yy=y;
  while (1) {
    if (lrand48()&128) ++xx;
    else ++yy;
    if ((xx>=n) || (yy>=shape[xx])) return 1;
    if ((*img)(xx,yy) == 1) return 0;
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
  return corner_plus_top(x,y);
}

int main (int argc, char **argv) {
  CL_Parser CLP (argc,argv,"n=2000");
  n = CLP.as_int('n');

  PointQueue queue;

  shape = new int[n];
  for (int i=0; i<n; ++i) shape[i]=0;
  maxx=1; maxy=1;

  img = new CoarseImage (n,n,(int)pow((double)n,.33),"A directed DLA cluster");
  img->onscreen();

  img->putpoint(0,0,1);
  queue << Point(1,0,log(drand48()))
        << Point(0,1,log(drand48()));

  srand48(time(0));

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
	  queue << Point(pt.x+1,pt.y,curtime-log(drand48()));
	if ( (pt.y<n-1) && ((*img)(pt.x,pt.y+1)==0) )
	  queue << Point(pt.x,pt.y+1,curtime-log(drand48()));
      } else {
	queue << Point(pt.x,pt.y,curtime-log(drand48()));
      }
    }
  }

  std::cout << *img;

  return 0;
}
