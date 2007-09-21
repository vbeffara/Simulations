#include <iostream>
#include <iomanip>
#include <vector>
#include <set>
#include <vb/PRNG.h>

#define N 10
#define PI 4*atan(1.0)

using namespace std;
using namespace vb;

typedef pair<int,double> coo;
typedef pair<coo,coo> ptt;

class pt : public ptt {
  public:
    pt (int i, int j, double x, double y) : ptt(coo(i,x), coo(j,y)) {};

    int &xi() { return first.first; }
    double &xf() { return first.second; }
    double x() const { return (double)first.first + first.second; }

    int &yi() { return second.first; }
    double &yf() { return second.second; }
    double y() const { return (double)second.first + second.second; }

    double dist2 (const pt o) {
      double dx = x() - o.x();
      double dy = y() - o.y();
      return dx*dx + dy*dy;
    }

    void step (const vector<double> &o);
    pt reverse ();
};
ostream &operator<< (ostream &os, const pt p) {
  os << p.x() << " " << p.y() << endl;
  return os;
}
void pt::step (const vector<double> &o) {
  int ij = N*xi() + yi();
  int r=0;
  double oo = o[ij];

  if (yf()==1) {
    r = 1;
    oo = -1/oo;
    yf() = xf();
    xf() = 0;
  } else if (xf()==1) {
    r = 2;
    yf() = 1-yf();
    xf() = 0;
  } else if (yf()==0) {
    r = 3;
    oo = -1/oo;
    yf() = 1-xf();
    xf() = 0;
  }

  // Avance

  int di=0, dj=0;

  if (oo < -yf()) {           // Vers le bas
    xf() = -yf()/oo;
    yf() = 0;
    dj = -1;
  } else if (oo < 1-yf()) {
    xf() = 1;
    yf() = yf() + oo;
    di = 1;
  } else {
    xf() = (1-yf())/oo;
    yf() = 1;
    dj = 1;
  }

  // Rotation inverse

  if (r==1) {
    int ti = di;
    di = dj;
    dj = -ti;
    double tx = xf();
    xf() = yf();
    yf() = 1-tx;
  } else if (r==2) {
    di = -di;
    dj = -dj;
    xf() = 1-xf();
    yf() = 1-yf();
  } else if (r==3) {
    int ti = di;
    di = -dj;
    dj = ti;
    double tx = xf();
    xf() = 1-yf();
    yf() = tx;
  }

  xf() -= di; xi() += di;
  yf() -= dj; yi() += dj;
}
pt pt::reverse () {
  if (xf()==0) { xf()=1; xi()--; }
  else if (xf()==1) { xf()=0; xi()++; }
  else if (yf()==0) { yf()=1; yi()--; }
  else if (yf()==1) { yf()=0; yi()++; }
  return (*this);
}
pt nopoint (-1,-1,-1,-1);

typedef pair<pt,pt> ptpair;

pt geodesique (pt p, const vector<double> &o, bool tracing=false) {
  set<pt> S;
  while (true) {
    if ( (p.xi()<0) || (p.xi()>=N) || (p.yi()<0) || (p.yi()>=N) ) break;
    if (tracing) cout << p;
    p.step(o);
    if (S.count(p)) break;
    S.insert(p);
  }
  if (tracing) cout << endl;

  if ( (p.xi()<0) || (p.xi()>=N) || (p.yi()<0) || (p.yi()>=N) ) return nopoint;
  else return p;
}
void leaf (pt p, const vector<double> &o, bool tracing=false) {
  geodesique (pt(p), o, tracing);
  p.reverse();
  geodesique (p, o, tracing);
}

set<ptpair> connections (const vector<double> &o) {
  set<ptpair> S;
  for (int i=0; i<N; ++i) {
    for (int j=0; j<N; ++j) {
      for (double x=0; x<=1; x+=.01) {
        pt p1 = geodesique (pt(i,j,x,0),o,false);
        pt p2 = geodesique (pt(i,j,x,0).reverse(),o,false);
        if (!((p1==nopoint)||(p2==nopoint))) {
          if ((S.count(ptpair(p1,p2)) == 0) && (S.count(ptpair(p2,p1)) == 0)) {
            pt p1 = geodesique (pt(i,j,x,0),o,true);
            pt p2 = geodesique (pt(i,j,x,0).reverse(),o,true);
            S.insert (ptpair(p1,p2));
          }
        }
      }
    }
  }
  cerr << endl;
  return S;
}

int main (int argc, char **argv) {
  PRNG prng;

  vector<double> o (N*N);
  for (int i=0; i<N*N; ++i) o[i] = tan(prng.uniform(PI));

  cout << setprecision(10);
  cerr << setprecision(10);

  set<ptpair> S = connections(o);
  set<pt> E;

  for (set<ptpair>::iterator i = S.begin(); i != S.end(); ++i) {
    geodesique (pt(i->first),o,true);
    geodesique (pt(i->second),o,true);
    E.insert(i->first);
    E.insert(i->second);
  }

  for (set<pt>::iterator i = E.begin(); i != E.end(); ++i) {
    cerr << (*i);
  }
}
