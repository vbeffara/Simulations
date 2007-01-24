/*
 * FPP.c - First Passage Percolation
 */

#include <vb.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

using namespace vb;

PointQueue queue;

double cost (void) {
  return -log(drand48());
}

int main (int argc, char **argv) {
  char title[100];
  double curtime;

  srand48(time(0));

  CL_Parser CLP (argc,argv,"n=200,i");
  int n = CLP.as_int('n');
  bool invasion = CLP.as_bool('i');

  snprintf (title,99,"A First-Passage Percolation cluster of radius %d", n);

  CoarseImage img (n,n,(int)pow((double)n,.33),title);

  img.onscreen();

  Point pt(n>>1,n>>1,+cost());
  queue << pt;

  curtime = 0;

  while (1) {
    queue >> pt;

    if (!(img(pt.x,pt.y))) {
      curtime = (invasion==1) ? 0.0 : pt.t;
      img << pt;
      if (!(img(pt.x+1,pt.y)))
	queue << Point(pt.x+1,pt.y,curtime+cost());
      if (!(img(pt.x-1,pt.y)))
	queue << Point(pt.x-1,pt.y,curtime+cost());
      if (!(img(pt.x,pt.y+1)))
	queue << Point(pt.x,pt.y+1,curtime+cost());
      if (!(img(pt.x,pt.y-1)))
	queue << Point(pt.x,pt.y-1,curtime+cost());
      
      if ( (pt.x==1) || (pt.y==1) || (pt.x==n-2) || (pt.y==n-2) )
	break;
    }
  }

  std::cout << img;

  return 0;
}
