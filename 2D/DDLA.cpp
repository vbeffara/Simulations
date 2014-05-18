/*
 * Directed DLA
 */

#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>
#include <vb/Point.h>

using namespace vb;
using namespace std;

int cursum;
int n,maxx,maxy;
double p;
bool s;

class DDLA : public CoarseImage {
public:
  DDLA (int n) : CoarseImage (n,n,"A directed DLA cluster", pow(double(n),.33)) {};

  void addapoint (coo z) {
    put (z,1);
    int x=z.x, y=z.y;
    if ( (x>0) && (y<n-1) && (at(z-coo(1,0)+coo(0,1))==1) && (at(z-coo(1,0))==0) )
      addapoint (z-coo(1,0));
    if ( (y>0) && (x<n-1) && (at(z+coo(1,0)-coo(0,1))==1) && (at(z-coo(0,1))==0) )
      addapoint (z-coo(0,1));
  }
};

DDLA *img;

void reshape (int x, int y) {
  if (x==maxx) ++maxx;
  if (y==maxy) ++maxy;
  if (x+y>cursum) cursum=x+y;
}

bool ok (coo z) {
  int xx=z.x, yy=z.y;
  coo zz=z;
  bool just_started=true;
  int ans=-1;

  while (ans == -1) {
    if (prng.bernoulli(p)) { ++xx; zz+=coo(1,0); } else { ++yy; zz+=coo(0,1); }
    // assert (zz==coo(xx,yy));

    if ((xx>=n) || (yy>=n) || (xx+yy > cursum)) ans = 1;
    if (img->at(zz) == 1) ans = 0;

    if (s && (!just_started) && (xx<n-1) && (yy<n-1) &&
        ((img->at(zz+coo(1,0))==1) || (img->at(zz+coo(0,1))==1))) ans = 0;
    just_started = false;
  }

  return ans;
}

int main (int argc, char **argv) {
  CL_Parser CLP (argc,argv,"n=2000,p=.5,s,t");
  n = CLP('n');
  p = CLP('p');
  s = CLP('s');

  PointQueue queue;

  maxx=1; maxy=1; cursum=0;

  img = new DDLA(n);
  img->show();

  img->put(0,1);
  queue << Point(coo(1,0),prng.exponential())
        << Point(coo(0,1),prng.exponential());

  int running=1;
  while (running) {
    Point pt (queue);

    if ((pt.z.x==n-1) || (pt.z.y==n-1)) running=0;
    else if (img->at(pt.z) == 0) {
      double curtime = pt.t;
      if (ok(pt.z)) {
        img->addapoint (pt.z);
        reshape (pt.z.x,pt.z.y);
        if ( (pt.z.x<n-1) && (img->at(pt.z+coo(1,0))==0) )
          queue << Point(pt.z+coo(1,0),curtime+prng.exponential()/p);
        if ( (pt.z.y<n-1) && (img->at(pt.z+coo(0,1))==0) )
          queue << Point(pt.z+coo(0,1),curtime+prng.exponential()/(1-p));
        if ( s && (pt.z.x>0) && (img->at(pt.z-coo(1,0))==0) )
          queue << Point(pt.z-coo(1,0),curtime+prng.exponential());
        if ( s && (pt.z.y>0) && (img->at(pt.z-coo(0,1))==0) )
          queue << Point(pt.z-coo(0,1),curtime+prng.exponential());
      } else {
        queue << Point(pt.z,curtime+prng.exponential());
      }
    }
  }

  img->output();
  return 0;
}
