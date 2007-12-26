/*
 * FPP.c - First Passage Percolation
 */

#include <vb.h>
#include <vb/Point.h>
#include <unistd.h>
#include <math.h>

using namespace vb;

PointQueue queue;

PRNG PR;

double cost (void) {
  return PR.exponential();
}

void spread (CoarseImage &img, double t, int x, int y) {
  if (!(img(x+1,y)))
    queue << Point(x+1,y,t+cost());
  if (!(img(x-1,y)))
    queue << Point(x-1,y,t+cost());
  if (!(img(x,y+1)))
    queue << Point(x,y+1,t+cost());
  if (!(img(x,y-1)))
    queue << Point(x,y-1,t+cost());
}

int main (int argc, char **argv) {
  char title[100];
  double curtime;

  CL_Parser CLP (argc,argv,"n=1000,i,2",
      "-n <size>, -i for invasion percolation, -2 for the two-step Eden model");
  int n = CLP.as_int('n');
  bool invasion = CLP.as_bool('i');
  bool twostep = CLP.as_bool('2');

  snprintf (title,99,"A First-Passage Percolation cluster of radius %d", n);

  CoarseImage img (n,n,(int)pow((double)n,.33),title);

  img.show();

  Point pt(n>>1,n>>1,+cost());
  queue << pt;
  if (twostep) queue << Point((n>>1)+1,n>>1,cost());

  curtime = 0;

  while (1) {
    queue >> pt;

    if (!(img(pt.x,pt.y))) {
      img << pt;

      curtime = (invasion==1) ? 0.0 : pt.t;
      int deg=1;
      if (twostep)
        deg = img(pt.x-1,pt.y) + img(pt.x+1,pt.y) + img(pt.x,pt.y+1) + img(pt.x,pt.y-1);

      for (int i=0; i<deg; ++i)
        spread (img,curtime,pt.x,pt.y);
      
      if (twostep) {
        if (img(pt.x-1,pt.y)) spread(img,curtime,pt.x-1,pt.y);
        if (img(pt.x+1,pt.y)) spread(img,curtime,pt.x+1,pt.y);
        if (img(pt.x,pt.y-1)) spread(img,curtime,pt.x,pt.y-1);
        if (img(pt.x,pt.y+1)) spread(img,curtime,pt.x,pt.y+1);
      }
      
      if ( (pt.x==1) || (pt.y==1) || (pt.x==n-2) || (pt.y==n-2) )
	break;
    }
  }

  std::cout << img;

  return 0;
}
