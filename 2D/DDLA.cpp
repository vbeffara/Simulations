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
bool t;

class DDLA : public CoarseImage {
public:
  DDLA (int n) : CoarseImage (n,n,"A directed DLA cluster", pow(double(n),.33)) {};

  void addapoint (coo z) {
    put (z,1);
    int x=real(z), y=imag(z);
    if ( (x>0) && (y<n-1) && (at(z-E1+E2)==1) && (at(z-E1)==0) )
      addapoint (z-E1);
    if ( (y>0) && (x<n-1) && (at(z+E1-E2)==1) && (at(z-E2)==0) )
      addapoint (z-E2);
  }
};

DDLA *img;

void reshape (int x, int y) {
  if (x==maxx) ++maxx;
  if (y==maxy) ++maxy;
  if (x+y>cursum) cursum=x+y;
}

bool ddla (int x, int y) {
  int xx=x, yy=y;
  bool just_started=true;
  int ans=-1;

  while (ans == -1) {
    if (prng.bernoulli(p)) ++xx; else ++yy;

    if ((xx>=n) || (yy>=n) || (xx+yy > cursum)) ans = 1;
    if ((*img)(xx,yy) == 1) ans = 0;

    if (s && (!just_started) && (xx<n-1) && (yy<n-1) &&
        (((*img)(xx+1,yy)==1) || ((*img)(xx,yy+1)==1))) ans = 0;
    just_started = false;
  }

  if (t && ans == 1) {
    int w=0, s=0;
    if ((x>0) && ((*img)(x-1,y)==1)) w=1;
    if ((y>0) && ((*img)(x,y-1)==1)) s=1;

    if ( (s==0) || ((w==1) && prng.bernoulli(p)) )
      cerr << x << " " << y << endl << x-1 << " " << y << endl << endl;
    else
      cerr << x << " " << y << endl << x << " " << y-1 << endl << endl;
  }

  return ans;
}

bool corner_plus_top (int x, int y) {
  if ( (y==maxy) || (x==maxx) ) return true;
  int bot   = (y==0  ?0:(*img)(x,y-1));
  int top   = (y==n-1?0:(*img)(x,y+1));
  int left  = (x==0  ?0:(*img)(x-1,y));
  int right = (x==n-1?0:(*img)(x+1,y));

  if (bot+top+left+right==2) return true;
  else return false;
}

bool ok (int x, int y) {
  return ddla(x,y);
  return corner_plus_top(x,y);
}

int main (int argc, char **argv) {
  CL_Parser CLP (argc,argv,"n=2000,p=.5,s,t");
  n = CLP('n');
  p = CLP('p');
  s = CLP('s');
  t = CLP('t');

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

    if ((real(pt.z)==n-1) || (imag(pt.z)==n-1)) running=0;
    else if ((*img)(real(pt.z),imag(pt.z))==0) {
      double curtime = pt.t;
      if (ok(real(pt.z),imag(pt.z))) {
        img->addapoint (pt.z);
        reshape (real(pt.z),imag(pt.z));
        if ( (real(pt.z)<n-1) && ((*img)(real(pt.z)+1,imag(pt.z))==0) )
          queue << Point(pt.z+E1,curtime+prng.exponential()/p);
        if ( (imag(pt.z)<n-1) && ((*img)(real(pt.z),imag(pt.z)+1)==0) )
          queue << Point(pt.z+E2,curtime+prng.exponential()/(1-p));
        if ( s && (real(pt.z)>0) && ((*img)(real(pt.z)-1,imag(pt.z))==0) )
          queue << Point(pt.z-E1,curtime+prng.exponential());
        if ( s && (imag(pt.z)>0) && ((*img)(real(pt.z),imag(pt.z)-1)==0) )
          queue << Point(pt.z-E2,curtime+prng.exponential());
      } else {
        queue << Point(pt.z,curtime+prng.exponential());
      }
    }
  }

  img->output();
  return 0;
}
