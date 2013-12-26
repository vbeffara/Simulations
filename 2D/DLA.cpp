#include <vb/CL_Parser.h>
#include <vb/CoarseImage.h>
#include <vb/PRNG.h>
#include <vb/Console.h>

using namespace vb;
using namespace std;

class KDTree {
public:
  KDTree () : size(0), children(2) {};

  void insert (coo _z, int d=0) {
    int c = (d==0 ? _z.x : _z.y);
    ++size;
    if (size==1) {
      z = _z;
      split = c;
    } else {
      int side = (c<split ? 0 : 1);
      if (children[side] == 0) children[side] = new KDTree;
      children[side] -> insert (_z,1-d);
    }
  }

  coo last (coo _z, int d=0, coo a=0) {
    if (size==0) return a;
    if (_z == z) return z;
    int c = (d==0 ? _z.x : _z.y);
    int side = (c<split ? 0 : 1);
    if (children[side] == 0) return z;
    return children[side] -> last (_z,1-d,z);
  }

  coo z;
  int split;
  int size;
private:
  vector<KDTree *> children;
};

int l1 (coo z) { return abs(z.x) + abs(z.y); }

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
  int n,r,deg;
  Console W;
  KDTree T;

  DLA (int n) : CoarseImage(n,n, "A DLA cluster", pow(n,.33)),
                n(n), r(1), deg(4) {
    z0 = coo(n/2,n/2);
    W.watch (T.size, "Number of particles");
    W.watch (r,      "Cluster radius");
  };

  void show ()   { W.show();   CoarseImage::show(); }
  void update () { W.update(); CoarseImage::update(); }

  char at (coo z) {
    int x=z.x, y=z.y;
    if ((x<-n/2) || (x>=n/2) || (y<-n/2) || (y>=n/2)) return 0;
    else return CoarseImage::at(z);
  }

  void put (coo z) {
    CoarseImage::put (z,1);
    T.insert(z);
    r = max (r, l1(z));
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
            z.x *= .9;
            z.y *= .9;
          }
        }
        d = min (2*d, l1(z));
      }

      put(z);
      if ((T.size%100)==0) update();
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
