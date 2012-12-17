
#include <vb/CL_Parser.h>
#include <vb/CoarseImage.h>
#include <vb/PRNG.h>
#include <vb/Watcher.h>

using namespace vb;
using namespace std;

typedef complex<int> place;

place jump (int l) {
  double theta = prng.uniform_real(0,2*M_PI);
  return place (l*cos(theta), l*sin(theta));
}

class DLA : public CoarseImage {
public:
  int n,r,pts;
  Watcher W;

  DLA (int n) : CoarseImage(n,n, str(fmt("A DLA cluster of size %d")%n), pow(n,.33)),
                n(n), r(1), pts(0) {
    W.watch (pts, "Number of particles");
    W.watch (r,   "Cluster radius");
  };

  void show () {
    W.show();
    CoarseImage::show();
  }

  void update () {
    W.update();
    CoarseImage::update();
  }

  char at (place z) {
    int x=real(z)+n/2, y=imag(z)+n/2;
    if ((x<0) || (x>=n) || (y<0) || (y>=n)) return 0;
    else return CoarseImage::at(x,y);
  }

  void putPoint(place z) {
    putpoint (real(z)+n/2,imag(z)+n/2, 1);
    r = max (r, abs(z));
    pts ++;
  }

  bool far (place z, int l) {
    for (int i=0; i<l; ++i) {
      if (at(z+place(-l+i,i)))  return false;
      if (at(z+place(i,l-i)))   return false;
      if (at(z+place(l-i,-i)))  return false;
      if (at(z+place(-i,-l+i))) return false;
    }
    return true;
  }

  int dist (place z, int d = 0) {
    if (abs(z)>2*r) return abs(z)-r;
    if (!far(z,1)) return 0;
    if (d==0) d = abs(real(z)) + abs(imag(z));
    while ((d>1) && !far(z,d)) d /= 2;
    return d;
  }

  void runDLA(){
    int d=0;
    place z(0,0);

    putPoint(z);

    while (r<n/2-1) {
      z = jump (2*r+2);

      while ((d=dist(z,d))>0) {
        if (d<6) {
          int dr = prng()%4;
          z += place (dx[dr],dy[dr]);
        } else {
          z += jump(d/2);
          if (abs(z) > 4*r) {
            real(z) *= .95;
            imag(z) *= .95;
          }
        }
        d = min (3*d, abs(z));
      }

      putPoint(z);
      if ((pts%100)==0) update();
    }
  }
};

int main(int argc, char ** argv) {
  CL_Parser CLP (argc,argv,"n=1000,l");

  DLA dla (CLP('n'));

  dla.show();
  if (CLP('l')) for (int i=-dla.n/4; i<dla.n/4; ++i) dla.putPoint(i);
  dla.runDLA();
  dla.output();

  return 0;
}
