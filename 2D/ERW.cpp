#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/CoarseImage.h>

using namespace vb;

int bump_square (coo z, double p, double q) {
  if (prng.bernoulli(p))   { if (z.y > abs(z.x)) return 3; if (z.y < -abs(z.x)) return 1; }
  if (prng.bernoulli(p*q)) { if (z.x > abs(z.y)) return 2; if (z.x < -abs(z.y)) return 0; }
  return -1;
}

int bump_lozenge (coo z, double p, double q) {
  if (prng.bernoulli(1-p)) return -1;
  if ((z.x>0)&&(z.y<0)) return (prng.bernoulli(.5) ? 2 : 1);
  if ((z.x<0)&&(z.y>0)) return (prng.bernoulli(.5) ? 0 : 3);
  if (prng.bernoulli(q)) {
    if ((z.x>0)&&(z.y>0)) return (prng.bernoulli(.5) ? 2 : 3);
    if ((z.x<0)&&(z.y<0)) return (prng.bernoulli(.5) ? 0 : 1);
  }
  return -1;
}

int bump_octo (coo z, double p, double q) {
  if (z.x >  2*abs(z.y)) return (prng.bernoulli(p) ? 2 : -1);
  if (z.x < -2*abs(z.y)) return (prng.bernoulli(p) ? 0 : -1);
  if (z.y >  2*abs(z.x)) return (prng.bernoulli(p) ? 3 : -1);
  if (z.y < -2*abs(z.x)) return (prng.bernoulli(p) ? 1 : -1);
  if (prng.bernoulli(p*q)) {
    if ((z.x>0)&&(z.y>0)) return (prng.bernoulli(.5) ? 2 : 3);
    if ((z.x>0)&&(z.y<0)) return (prng.bernoulli(.5) ? 2 : 1);
    if ((z.x<0)&&(z.y<0)) return (prng.bernoulli(.5) ? 0 : 1);
    if ((z.x<0)&&(z.y>0)) return (prng.bernoulli(.5) ? 0 : 3);
  }
  return -1;
}

int bump_dent (coo z, double p, double q) {
  if ((z.y>z.x) && (z.y<2*z.x)) return (prng.bernoulli(p*2) ? 2 : -1);
  if ((z.x>z.y) && (z.x<2*z.y)) return (prng.bernoulli(p*2) ? 3 : -1);
  return bump_square (z,p,q);
}

int bump_iso (coo z, double p, double q) {
  if (z==coo(0)) return -1; if (prng.bernoulli(1-p)) return -1;
  if ((z.x>=0) && (z.y>=0)) return (prng.bernoulli(double(z.x)/(z.x+z.y)) ? 2 : 3);
  if ((z.x>=0) && (z.y<=0)) return (prng.bernoulli(double(z.x)/(z.x-z.y)) ? 2 : 1);
  if ((z.x<=0) && (z.y>=0)) return (prng.bernoulli(double(z.x)/(z.x-z.y)) ? 0 : 3);
  if ((z.x<=0) && (z.y<=0)) return (prng.bernoulli(double(z.x)/(z.x+z.y)) ? 0 : 1);
  return 0; // should not happen.
}

int bump_lozenge_2 (coo z, double p, double q) {
  if (prng.bernoulli(1-p)) return -1;
  if ((z.x>0)&&(z.y<0)) return (prng.bernoulli(.5) ? 2 : 1);
  if ((z.x<0)&&(z.y>0)) return (prng.bernoulli(.5) ? 0 : 3);
  if (prng.bernoulli(q)) {
    if ((z.x>0)&&(z.y>0)) return ( (z.x+z.y)%2 ? 2 : 3);
    if ((z.x<0)&&(z.y<0)) return ((-z.x-z.y)%2 ? 0 : 1);
  }
  return -1;
}

class ERW : public CoarseImage {
public:
  double p,q,r;

  ERW (int n, CL_Parser CLP) : CoarseImage (n,n, "An excited random walk", pow(n,.33)),
                               p(CLP('p')), q(CLP('q')), r(CLP('r')) {};

  void run () {
    int n=true_width, d;

    for (int x=0; x<n/4; ++x) {
      for (int y=0; y<n/4-x; ++y) {
        put (coo(x+n/2, y+n/2), 1);
        put (coo(x+n/2, -y+n/2), 1);
        put (coo(-x+n/2, -y+n/2), 1);
        put (coo(-x+n/2, y+n/2), 1);
      }
    }

    coo z (0,0);
    while (contains(z+coo(n/2,n/2))) {
      int first = 1 - at(z+coo(n/2,n/2));
      if (prng.bernoulli(r)) put (z+coo(n/2,n/2),1);

      if (first) d = bump_square (z,p,q); else d = prng()&3;
      if (d>=0) { z += dz[d]; }
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
