
#include <vb/CL_Parser.h>
#include <vb/CoarseImage.h>
#include <vb/PRNG.h>
#include <vb/Watcher.h>

using namespace vb;
using namespace std;

int l1 (coo z) { return abs(real(z)) + abs(imag(z)); }

coo jump (int l, int deg=4) {
  double theta = prng.uniform_real(0,2*M_PI);
  double x = l*cos(theta);
  double y = l*sin(theta);
  if (deg!=6) return coo (x,y);
  double xx = x + y/sqrt(3.0);
  double yy = 2.0*y/sqrt(3.0);
  return coo (xx,yy);
}

class DLA : public CoarseImage {
public:
  int n,r,pts,deg;
  Watcher W;

  DLA (int n) : CoarseImage(n,n, str(fmt("A DLA cluster of size %d")%n), pow(n,.33)),
                n(n), r(1), pts(0), deg(4) {
    z0 = coo(n/2,n/2);
    W.watch (pts, "Number of particles");
    W.watch (r,   "Cluster radius");
  };

  void show ()   { W.show();   CoarseImage::show(); }
  void update () { W.update(); CoarseImage::update(); }

  char at (coo z) {
    int x=real(z), y=imag(z);
    if ((x<-n/2) || (x>=n/2) || (y<-n/2) || (y>=n/2)) return 0;
    else return CoarseImage::at(z);
  }

  void put (coo z) {
    CoarseImage::put (z,1);
    r = max (r, l1(z));
    pts ++;
  }

  bool neighbor (coo z) {
    for (int i=0; i<deg; ++i)
      if (at(z+dz[i])) return true;
    return false;
  }

  bool far (coo z, int l) {
    for (int i=0; i<l; ++i) {
      int j = l-i;
      if (at(z+coo(i,j)))   return false;
      if (at(z+coo(j,-i)))  return false;
      if (at(z+coo(-i,-j))) return false;
      if (at(z+coo(-j,i)))  return false;
    }
    return true;
  }

  int dist (coo z, int d) {
    int l = l1(z);
    if (l>2*r) return l-r;
    while ((d>1) && !far(z,d)) d /= 2;
    return d;
  }

  void runDLA (){
    put(0);

    while (r<n/2-1) {
      coo z = jump (2*r+2, deg);
      int d = r+2;

      while (!neighbor(z)) {
        d = dist(z,d);
        if (d<10) {
          z += dz[prng()%deg];
        } else {
          z += jump(d/3, deg);
          if (l1(z) > 100*r) {
            real(z) *= .9;
            imag(z) *= .9;
          }
        }
        d = min (2*d, l1(z));
      }

      put(z);
      if ((pts%100)==0) update();
    }
  }
};

int main (int argc, char ** argv) {
  CL_Parser CLP (argc,argv,"n=1000,l,d=4");

  DLA dla (CLP('n'));
  dla.deg = CLP('d');

  dla.show();
  if (CLP('l')) for (int i=-dla.n/4; i<dla.n/4; ++i) dla.put(i);

  dla.runDLA();
  dla.output();

  return 0;
}
