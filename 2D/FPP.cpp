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
  char ch;
  char title[100];
  double curtime;

  srand48(time(0));

  int n = 200;
  int invasion = 0;

  while ((ch = getopt(argc,argv,"hn:i")) != -1) {
    switch (ch) {
    case 'n':
      n = atoi (optarg);
      break;
    case 'i':
      invasion = 1;
      break;
    case 'h':
      std::cerr << "Syntax: " << argv[0] << " [-n size] [-i]\n";
      std::cerr << "  -i : do invasion percolation instead.\n";
      exit (1);
      break;
    }
  }

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
