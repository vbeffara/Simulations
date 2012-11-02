
#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace vb;

Image *img;
char *m;
long n,radius;
long minx,maxx,miny,maxy,mx,my;

void putPoint(long x, long y)
{
  long t,cx,cy;

  if ((x<0) || (x>=n) || (y<0) || (y>=n)) return; /* Should never happen */

  img->putpoint (x,y,1); /* Put the point */

  t=n>>1; cx=0; cy=0; /* Do the clever thing */
  while (t>1) {
    if (x&t) cx|=t;
    if (y&t) cy|=t;
    m[(cx + (t>>1)) + n*(cy + (t>>1))] |= 128;
    t >>= 1;
  }

  if (x<minx) minx=x; /* Update radius & co */
  if (x>maxx) maxx=x;
  if (y<miny) miny=y;
  if (y>maxy) maxy=y;
  mx = (minx+maxx)>>1;
  my = (miny+maxy)>>1;
  if (maxx-minx > maxy-miny)
    radius = (maxx-minx)>>1;
  else
    radius = (maxy-miny)>>1;
}

long dist (long x, long y)
{
  long point,d,t,u,close,cx,cy;

  d=0;

  if ((t=x-maxx)>d) d=t;
  if ((t=y-maxy)>d) d=t;
  if ((t=minx-x)>d) d=t;
  if ((t=miny-y)>d) d=t;

  if (d>1) return d;

  /* Hence now we are in the big box. */

  point = x+y*n;

  if (((*img)(point))&1) return -1; /* Should not happen  */

  if ((x>0) && (((*img)(point-1))&1)) { return 0; } /* = neighbour */
  if ((y>0) && (((*img)(point-n))&1)) { return 0; } /* = neighbour */
  if ((x<n-1) && (((*img)(point+1))&1)) { return 0; } /* = neighbour */
  if ((y<n-1) && (((*img)(point+n))&1)) { return 0; } /* = neighbour */

  /* OK we are in the big box but not neighbour. */

  t=n>>1; cx=0; cy=0; close=1;

  while ((t>1)&&close) {
    if (x&t) cx|=t;
    if (y&t) cy|=t;
    if (!(m[(cx + (t>>1)) + n*(cy + (t>>1))] & 128)) close=0;
    t >>= 1;
  }

  /* We are in an empty box of radius t around (cx+t,cy+t). */

  d = abs(x-cx-t);
  if ((u=abs(y-cy-t))>d) d=u;
  d = t-d;
  if (d==0) d=1; /* Can this happen ? Who knows ... */

  return d;
}

long bigrand (long s)
{
  return -s + ((prng()) % (2*s+1));
}

void runDLA(){
  long x,y,pts;
  long sradius;
  long d,r,step;

  minx = n/2; maxx = n/2;
  miny = n/2; maxy = n/2;
  mx = n/2; my = n/2;
  x=n/2; y=n/2;
  putPoint(n/2,n/2);
  pts=1;

  while ((x>0)&&(x<n-1)&&(y>0)&&(y<n-1)) {
    sradius = 2*radius + 5; /* Mouais bof ... */

    switch (prng() & 3) {
    case 0:
      x = mx + bigrand(sradius);
      y = my - sradius;
      break;
    case 1:
      x = mx + bigrand(sradius);
      y = my + sradius;
      break;
    case 2:
      y = my + bigrand(sradius);
      x = mx - sradius;
      break;
    case 3:
      y = my + bigrand(sradius);
      x = mx + sradius;
      break;
    }

    while ((d=dist(x,y))) {
      if (d==1) {
        r = prng()%4;
        x += dx[r];
        y += dy[r];
      } else {
        step = (d>>2) + 1; /* Mouais bof ... */
        /* fprintf(stderr,"%10ld%10ld\r",d,step); */
        switch (prng()%4) {
        case 0:
          x += step;
          y += bigrand(step);
          break;
        case 1:
          x -= step;
          y += bigrand(step);
          break;
        case 2:
          y += step;
          x += bigrand(step);
          break;
        case 3:
          y -= step;
          x += bigrand(step);
          break;
        }
      }
    }

    putPoint(x,y);
    pts++;
    if ((pts%100)==0) fprintf (stderr,"%ld pts, r=%ld\r",pts,radius);
  }
  fprintf (stderr,"\n");
}

int main(int argc, char ** argv)
{
  /* arguments -> taille du terrain */

  CL_Parser CLP (argc,argv,"n=8");
  n = 1 << int(CLP('n'));

  /* Initialisations */

  img = new Image(n,n, str(fmt("A DLA cluster of size %d")%n));
  m = new char[n*n];

  /* Simulation */

  img->show();
  runDLA();

  return 0;
}
