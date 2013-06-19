// SLE - Schramm's Stochastic Loewner Evolution - v2 (multiscale)

#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace std;
using namespace vb;

#define DONTKNOW  Color(0)
#define INSIDE    Color(0)
#define LEFTSIDE  Color(100)
#define RIGHTSIDE Color(150)

class SLE : public Image {
public:
  SLE (CL_Parser &CLP);

  Color quick (int x, int y, int l);
  Color compute (coo z);
  void edge_detect ();

  int n;
  double kappa;
  vector<double> W,Wmin,Wmax;
  bool q;
};

SLE::SLE (CL_Parser &CLP) : Image(2*int(CLP('n')),CLP('n'),"SLE"), n(h()*h()), kappa(CLP('k')), W(n,0), q(CLP('q')) {
  for (int i=1;i<n;i++) W[i] = W[i-1] + prng.gaussian();

  double d = W[n-1]/n; for (int i=1; i<n;i++) W[i] -= i*d;

  Wmin.resize(n+1); Wmax.resize(n+1);
  Wmax[n]=W[n-1]; Wmin[n]=W[n-1];

  for (int i=n-1; i>0; i--) {
    Wmin[i] = min (W[i-1], Wmin[i+1]);
    Wmax[i] = max (W[i-1], Wmax[i+1]);
  }

  Wmin[0]=Wmin[1]; Wmax[0]=Wmax[1];
}

Color SLE::quick (int x, int y, int l) {
  double a=x-w()/2, b=y;
  int k;

  for (k=0; (k<n) && (b>0); k+=l) {
    if (a<Wmin[k]) return LEFTSIDE;
    if (a>Wmax[k]) return RIGHTSIDE;
    for (int i=0; i<1; ++i) {
      double d = (2/kappa) / ((a-W[k])*(a-W[k]) + b*b);
      a += (a-W[k])*d*l;
      b -= b*d*l;
    }
  }
  if (k>n) k=n;
  if ((l>1) && (k==n)) return DONTKNOW;
  if (a<W[k]) return LEFTSIDE; else return RIGHTSIDE;
}

Color SLE::compute (coo z) {
  int x=real(z), y=imag(z);
  if (q) {
    for (int l=1024; l>1; l/=2) {
      if (quick(x+l,y,l) == LEFTSIDE) return LEFTSIDE;
      if (quick(x-l,y,l) == RIGHTSIDE) return RIGHTSIDE;
    }
  }

  return quick (x,y,1);
}

void SLE::edge_detect () {
  char self,right,down;

  for (int y=1; y<h(); y++) {
    for (int x=0; x<w()-1; x++) {
      self = at(coo(x,y));
      if (x<w()-1) {
        right = at(coo(x+1,y));
      } else {
        right = self;
      }
      if (y<h()-1) {
        down = at(coo(x,y+1));
      } else {
        down = self;
      }

      if ((self!=right)||(self!=down)) {
        putpoint (x,y,INSIDE);
      }
    }
  }
}

int main (int argc, char ** argv) {
  CL_Parser CLP (argc,argv,"n=300,k=2.666666666667,r=0,l=1,q");

  SLE sle (CLP);

  int r = CLP('r'); if (r) prng.seed(r);

  for (int i=0;i<sle.w();i++) sle.putpoint (i,0,INSIDE);

  sle.show();
  sle.tessel (0,1,sle.w()-1,sle.h()-1);
  sle.edge_detect();
  sle.output_png();

  return 0;
}
