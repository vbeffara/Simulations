#include <iostream>
#include <vector>
#include <vb/PRNG.h>

#define N 10
#define PI 4*atan(1.0)

using namespace std;
using namespace vb;

class pt {
  public:
    int i;
    int j;
    double x;
    double y;

    pt (int ii, int jj, double xx, double yy) : i(ii), j(jj), x(xx), y(yy) {};

    bool operator== (const pt o) const {
      return ( (i==o.i) && (j==o.j) && (x==o.x) && (y==o.y) );
    }

    double dist2 (const pt o) {
      double dx = (double)i + x - (double)(o.i) - o.x;
      double dy = (double)j + y - (double)(o.j) - o.y;
      return dx*dx + dy*dy;
    }

    void step (const vector<double> &o);
};

void pt::step (const vector<double> &o) {
  int ij = N*i+j;
  int r=0;
  double oo = o[ij];

  if (y==1) {
    r = 1;
    oo += PI/2;
    y = x;
    x = 0;
  } else if (x==1) {
    r = 2;
    y = 1-y;
    x = 0;
  } else if (y==0) {
    r = 3;
    oo -= PI/2;
    y = 1-x;
    x = 0;
  }

  // Avance

  int di=0, dj=0;
  double t = tan (oo);

  if (t < -y) {           // Vers le bas
    x = -y/t;
    y = 0;
    dj = -1;
  } else if (t < 1-y) {
    x = 1;
    y = y + t;
    di = 1;
  } else {
    x = (1-y)/t;
    y = 1;
    dj = 1;
  }

  // Rotation inverse

  if (r==1) {
    int ti = di;
    di = dj;
    dj = -ti;
    double tx = x;
    x = y;
    y = 1-tx;
  } else if (r==2) {
    di = -di;
    dj = -dj;
    x = 1-x;
    y = 1-y;
  } else if (r==3) {
    int ti = di;
    di = -dj;
    dj = ti;
    double tx = x;
    x = 1-y;
    y = tx;
  }

  x -= di; i += di;
  y -= dj; j += dj;
}

pt geodesique (pt p, const vector<double> &o, bool tracing=false) {
  for (int step=0; step<300; ++step) {
    if (tracing)
      cout << (double)p.i + p.x << " " << (double)p.j + p.y << endl;
    p.step(o);
    if ( (p.i<0) || (p.i>=N) || (p.j<0) || (p.j>=N) ) break;
  }

  if (tracing) cout << endl;
  return p;
}

pt reverse (pt p) {
  if (p.x==0) { p.x=1; p.i--; }
  else if (p.x==1) { p.x=0; p.i++; }
  else if (p.y==0) { p.y=1; p.j--; }
  else if (p.y==1) { p.y=0; p.j++; }
  return p;
}

void hair (pt p, pt trap, const vector<double> &o) {
    pt p1 = geodesique (p,o,false);
    if (p1.dist2(trap) < 1e-3) {
      geodesique (p,o,true);
      geodesique (reverse(p),o,true);
    }
}

int main (int argc, char **argv) {
  PRNG prng;

  vector<double> o (N*N);
  for (int i=0; i<N*N; ++i) o[i] = prng.uniform(2*PI);
  while (true) {
    double o1 = o[(N/2)*N + (N/2)] = prng.uniform(2*PI);
    double o2 = o[(N/2-1)*N + (N/2)] = prng.uniform(2*PI);
    double o3 = o[(N/2-1)*N + (N/2-1)] = prng.uniform(2*PI);
    double o4 = o[(N/2)*N + (N/2-1)] = prng.uniform(2*PI);

    if ( (tan(o1)<0) && (tan(o2)>0) && (tan(o3)<0) && (tan(o4)>0) ) break;
  } 

  pt trap (N/2,N/2,0,0);
  for (double x=0; x<1.0; x+=.01) {
    hair (pt(N/2,N/2,1,x),trap,o);
    hair (pt(N/2,N/2,x,1),trap,o);

    hair (pt(N/2-1,N/2,x,1),trap,o);
    hair (pt(N/2-1,N/2,0,x),trap,o);

    hair (pt(N/2-1,N/2-1,0,x),trap,o);
    hair (pt(N/2-1,N/2-1,x,0),trap,o);

    hair (pt(N/2,N/2-1,x,0),trap,o);
    hair (pt(N/2,N/2-1,1,x),trap,o);
  }
}
