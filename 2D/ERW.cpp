/*
 * Excited random walk in the plane.
 */

#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/CoarseImage.h>

using namespace vb;

int bump_square (int x, int y, double p, double q);
int bump_lozenge (int x, int y, double p, double q);
int bump_octo (int x, int y, double p, double q);
int bump_dent (int x, int y, double p, double q);
int bump_iso (int x, int y, double p, double q);
int bump_lozenge_2 (int x, int y, double p, double q);

class ERW : public CoarseImage {
public:
  double p,q,r;

  ERW (int n, CL_Parser CLP) : CoarseImage (n,n, "An excited random walk", pow(n,.33)),
                               p(CLP('p')), q(CLP('q')), r(CLP('r')) {};

  void run () {
    int x=0, y=0, n=true_width, d;

    for (int x=0; x<n/4; ++x) {
      for (int y=0; y<n/4-x; ++y) {
        putpoint (x+n/2, y+n/2, 1, 0);
        putpoint (x+n/2, -y+n/2, 1, 0);
        putpoint (-x+n/2, -y+n/2, 1, 0);
        putpoint (-x+n/2, y+n/2, 1, 0);
      }
    }

    while ((x>=-n/2)&&(y>=-n/2)&&(x<n/2)&&(y<n/2)) {
      int first = 1 - at(x+n/2,y+n/2);
      if (prng.bernoulli(r)) putpoint (x+n/2,y+n/2,1);

      if (first) d = bump_square (x,y,p,q); else d = prng()&3;
      if (d>=0) { x += dx[d]; y += dy[d]; }
    }
  }
};

int main (int argc, char ** argv) {
  CL_Parser CLP (argc,argv,"n=1000,s=1,p=1,q=1,r=1");
  int n = CLP('n');
  ERW img(n,CLP);

  prng.seed(int(CLP('s')));
  img.show();
  img.run();
  return 0;
}

int bump_square (int x, int y, double p, double q) {
  if (prng.bernoulli(p)) {
    if (y >  abs(x)) return 3;
    if (y < -abs(x)) return 1;
  }
  if (prng.bernoulli(p*q)) {
    if (x >  abs(y)) return 2;
    if (x < -abs(y)) return 0;
  }
  return -1;
}

int bump_lozenge (int x, int y, double p, double q) {
  if (prng.bernoulli(1-p)) return -1;
  if ((x>0)&&(y<0)) return (prng.bernoulli(.5) ? 2 : 1);
  if ((x<0)&&(y>0)) return (prng.bernoulli(.5) ? 0 : 3);
  if (prng.bernoulli(q)) {
    if ((x>0)&&(y>0)) return (prng.bernoulli(.5) ? 2 : 3);
    if ((x<0)&&(y<0)) return (prng.bernoulli(.5) ? 0 : 1);
  }
  return -1;
}

int bump_octo (int x, int y, double p, double q) {
  if (x >  2*abs(y)) return (prng.bernoulli(p) ? 2 : -1);
  if (x < -2*abs(y)) return (prng.bernoulli(p) ? 0 : -1);
  if (y >  2*abs(x)) return (prng.bernoulli(p) ? 3 : -1);
  if (y < -2*abs(x)) return (prng.bernoulli(p) ? 1 : -1);
  if (prng.bernoulli(p*q)) {
    if ((x>0)&&(y>0)) return (prng.bernoulli(.5) ? 2 : 3);
    if ((x>0)&&(y<0)) return (prng.bernoulli(.5) ? 2 : 1);
    if ((x<0)&&(y<0)) return (prng.bernoulli(.5) ? 0 : 1);
    if ((x<0)&&(y>0)) return (prng.bernoulli(.5) ? 0 : 3);
  }
  return -1;
}

int bump_dent (int x, int y, double p, double q) {
  if ((y>x) && (y<2*x)) return (prng.bernoulli(p*2) ? 2 : -1);
  if ((x>y) && (x<2*y)) return (prng.bernoulli(p*2) ? 3 : -1);
  return bump_square (x,y,p,q);
}

int bump_iso (int x, int y, double p, double q) {
  if ((x==0) && (y==0)) return -1;
  if (prng.bernoulli(1-p)) return -1;
  if ((x>=0) && (y>=0)) return (prng.bernoulli(double(x)/(x+y)) ? 2 : 3);
  if ((x>=0) && (y<=0)) return (prng.bernoulli(double(x)/(x-y)) ? 2 : 1);
  if ((x<=0) && (y>=0)) return (prng.bernoulli(double(x)/(x-y)) ? 0 : 3);
  if ((x<=0) && (y<=0)) return (prng.bernoulli(double(x)/(x+y)) ? 0 : 1);
}

int bump_lozenge_2 (int x, int y, double p, double q) {
  if (prng.bernoulli(1-p)) return -1;
  if ((x>0)&&(y<0)) return (prng.bernoulli(.5) ? 2 : 1);
  if ((x<0)&&(y>0)) return (prng.bernoulli(.5) ? 0 : 3);
  if (prng.bernoulli(q)) {
    if ((x>0)&&(y>0)) return ( (x+y)%2 ? 2 : 3);
    if ((x<0)&&(y<0)) return ((-x-y)%2 ? 0 : 1);
  }
  return -1;
}
