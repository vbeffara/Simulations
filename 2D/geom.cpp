#include <iostream>
#include <vector>
#include <set>
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

    bool operator< (const pt o) const {
      if (i<o.i) return true; else if (i>o.i) return false;
      if (j<o.j) return true; else if (j>o.j) return false;
      if (x<o.x) return true; else if (x>o.x) return false;
      if (y<o.y) return true; else return false;
    }

    double dist2 (const pt o) {
      double dx = (double)i + x - (double)(o.i) - o.x;
      double dy = (double)j + y - (double)(o.j) - o.y;
      return dx*dx + dy*dy;
    }

    void step (const vector<double> &o);
    void reverse ();
};
ostream &operator<< (ostream &os, const pt p) {
  os << (double)p.i + p.x << " " << (double)p.j + p.y << endl;
  return os;
}
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
void pt::reverse () {
  if (x==0) { x=1; i--; }
  else if (x==1) { x=0; i++; }
  else if (y==0) { y=1; j--; }
  else if (y==1) { y=0; j++; }
}
pt nopoint (-1,-1,-1,-1);

pt geodesique (pt p, const vector<double> &o, bool tracing=false) {
  set<pt> S;
  while (true) {
    if (tracing) cout << (double)p.i + p.x << " " << (double)p.j + p.y << endl;
    p.step(o);
    if ( (p.i<0) || (p.i>=N) || (p.j<0) || (p.j>=N) ) break;
    if (S.count(p)) break;
    S.insert(p);
  }
  if (tracing) cout << endl;

  if ( (p.i<0) || (p.i>=N) || (p.j<0) || (p.j>=N) ) return nopoint;
  else return p;
}
void hair (pt p, pt trap, const vector<double> &o) {
  pt p1 = geodesique (pt(p),o,false);
  if (p1.dist2(trap) == 0) {
    geodesique (pt(p),o,true);
    p.reverse();
    geodesique (p,o,true);
  }
}
void octopus (int i, int j, vector<double> &o, PRNG &prng) {
  while (true) {
    double o1 = o[(i)*N + (j)] = prng.uniform(2*PI);
    double o2 = o[(i-1)*N + (j)] = prng.uniform(2*PI);
    double o3 = o[(i-1)*N + (j-1)] = prng.uniform(2*PI);
    double o4 = o[(i)*N + (j-1)] = prng.uniform(2*PI);

    if ( (tan(o1)<0) && (tan(o2)>0) && (tan(o3)<0) && (tan(o4)>0) ) break;
  } 

  pt trap (i,j,0,0);
  for (double x=0; x<1.0; x+=.01) {
    hair (pt(i,j,1,x),trap,o);
    hair (pt(i,j,x,1),trap,o);

    hair (pt(i-1,j,x,1),trap,o);
    hair (pt(i-1,j,0,x),trap,o);

    hair (pt(i-1,j-1,0,x),trap,o);
    hair (pt(i-1,j-1,x,0),trap,o);

    hair (pt(i,j-1,x,0),trap,o);
    hair (pt(i,j-1,1,x),trap,o);
  }
}
set<pt> fixpoints (const vector<double> &o) {
  set<pt> S;
  for (int i=0; i<N; ++i) {
    for (int j=0; j<N; ++j) {
      for (double x=0; x<=1; x+=.01) {
        S.insert ( geodesique (pt(i,j,x,0),o,false) );
        S.insert ( geodesique (pt(i,j,x,1),o,false) );
        S.insert ( geodesique (pt(i,j,0,x),o,false) );
        S.insert ( geodesique (pt(i,j,1,x),o,false) );
      }
    }
  }
  return S;
}

int main (int argc, char **argv) {
  PRNG prng;

  vector<double> o (N*N);
  for (int i=0; i<N*N; ++i) o[i] = prng.uniform(2*PI);

  set<pt> S = fixpoints(o);
  for (set<pt>::iterator i = S.begin(); i != S.end(); ++i) {
    if (!(*i == nopoint)) geodesique (pt(*i),o,true);
  }
}
