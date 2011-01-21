#include <iostream>
#include <iomanip>

#include <vb/config.h>
#include <vb/Map.h>

using namespace std;
using namespace vb;

template <typename T1, typename T2> ostream &operator<< (ostream &os, pair<T1,T2> p) {
  os << '(' << p.first << ',' << p.second << ')';
  return os;
}

double fg (const Vector &x, Vector &g, void *context) {
  Map *m = (Map *) context;
  double c = 0.0;

  fill (g.begin(), g.end(), 0.0);

  for (int i=0; i < m->n; ++i) {
    foreach (int j, m->v[i]->adj) {
      double dx = x[3*j]-x[3*i];
      double dy = x[3*j+1]-x[3*i+1];
      double l = sqrt(dx*dx + dy*dy);
      double sr = x[3*i+2] + x[3*j+2];
      double lsr = l-sr;
      double lsrl = lsr/l;

      c += lsr * lsr;

      g[3*i]   -= lsrl*dx;
      g[3*i+1] -= lsrl*dy;

      if (!(m->bd[i]))
        g[3*i+2] -= lsr;
    }
  }

  return c;
}



int main () {
  Map m (13);

  m << Edge(0,1) << Edge(0,3) << Edge(0,5)
    << Edge(1,2) << Edge(1,4) << Edge(1,6) << Edge(1,3) << Edge(1,0)
    << Edge(2,7) << Edge(2,4) << Edge(2,1)
    << Edge(3,0) << Edge(3,1) << Edge(3,6) << Edge(3,5)
    << Edge(4,1) << Edge(4,2) << Edge(4,7) << Edge(4,6)
    << Edge(5,0) << Edge(5,3) << Edge(5,6) << Edge(5,8) << Edge(5,10)
    << Edge(6,1) << Edge(6,4) << Edge(6,7) << Edge(6,9) << Edge(6,11) << Edge(6,8) << Edge(6,5) << Edge(6,3)
    << Edge(7,12) << Edge(7,9) << Edge(7,6) << Edge(7,4) << Edge(7,2)
    << Edge(8,5) << Edge(8,6) << Edge(8,11) << Edge(8,10)
    << Edge(9,6) << Edge(9,7) << Edge(9,12) << Edge(9,11)
    << Edge(10,5) << Edge(10,8) << Edge(10,11)
    << Edge(11,10) << Edge(11,8) << Edge(11,6) << Edge(11,9) << Edge(11,12)
    << Edge(12,11) << Edge(12,9) << Edge(12,7);

  m.inscribe(m.face(Edge(0,m.v[0]->adj.back())));

  for (int i=0; i<m.n; ++i) m.v[i]->r=.5/sqrt((double)m.n);

  m.show();
  m.balance();
  m.barycentric(); m.inscribe(m.face(Edge(0,m.v[0]->adj.back()))); m.balance();
  m.barycentric(); m.inscribe(m.face(Edge(0,m.v[0]->adj.back()))); m.balance();
  m.barycentric(); m.inscribe(m.face(Edge(0,m.v[0]->adj.back()))); m.balance();
  //m.barycentric(); m.inscribe(m.face(Edge(0,m.v[0]->adj.back()))); m.balance();

  std::list<int> bord = m.face (Edge(0,m.v[0]->adj.back()));

  bool circlepack = false;
  if (circlepack) {
    // 7 seconds total on seltz.
    m.circlepack (6,0,bord);
  } else {
    // 3.6 seconds total on seltz.
    foreach (bool b, m.bd) b = false;
    foreach (int i, bord) m.bd[i] = true;

    Vector x(3*m.n);

    for (int i=0; i<m.n; ++i) {
      x[3*i]   = m.v[i]->z.real();
      x[3*i+1] = m.v[i]->z.imag();
      x[3*i+2] = 1.0 / sqrt(double(m.n));
    }

    Minimizer M (3*m.n, fg, &m);
    M.os = &cerr;
    M.minimize_qn (x);

    for (int i=0; i<m.n; ++i) {
      m.v[i]->z = cpx (M.x[3*i],M.x[3*i+1]);
      m.v[i]->r = M.x[3*i+2];
    }
    // do nothing
  }

  Figure f; m.plot_circles (f);
  f.show(); f.pause(); f.run();
}
