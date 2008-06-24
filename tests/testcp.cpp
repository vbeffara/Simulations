
#include <vb/minimize.h>
#include <vb/Map.h>
#include <iostream>

using namespace vb;
using namespace std;

int center = 6;
int infinity = 13;

double fg_balance (const Vector<double> &x, Vector<double> &g, void *context) {
  Map *m = (Map *) context;
  double c = 0.0;

  for (int i=0; i < m->n; ++i) {
    g[2*i] = 0;
    g[2*i+1] = 0;

    for (adj_list::iterator j = m->v[i]->adj.begin(); j != m->v[i]->adj.end(); ++j) {
      c += (x[2*(*j)] - x[2*i]) * (x[2*(*j)] - x[2*i]);
      c += (x[2*(*j)+1] - x[2*i+1]) * (x[2*(*j)+1] - x[2*i+1]);
      if (!(m->bd[i])) {
        g[2*i]    -= x[2*(*j)] - x[2*i];
        g[2*i+1]    -= x[2*(*j)+1] - x[2*i+1];
      }
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
    //<< Edge(13,0) << Edge(13,1) << Edge(13,2) << Edge(13,5) << Edge(13,7) << Edge(13,10) << Edge(13,11) << Edge(13,12);

  m.barycentric();
  m.barycentric();
  m.barycentric();
  m.barycentric();
  m.inscribe(m.face(Edge(1,m.v[1]->adj.back())));

  bool balance = false;

  if (balance) {
    m.balance();
  } else {
    Vector<double> x(2*m.n);
    Vector<double> degree(2*m.n);

    for (int i=0; i<m.n; ++i) {
      x[2*i]        = m.v[i]->z.real();
      x[2*i+1]      = m.v[i]->z.imag();
      double dd = m.v[i]->adj.size();
      if (dd>0) {
        degree[2*i]   = 1.0 / sqrt(dd);
        degree[2*i+1] = 1.0 / sqrt(dd);
      }
    }
    
    Minimizer<double> MM (2*m.n, fg_balance, &m);
    MM.minimize_bfgs (x,degree);
    x = MM.x;

    for (int i=0; i<m.n; ++i) { m.v[i]->z.real()=x[2*i]; m.v[i]->z.imag()=x[2*i+1]; }
  }

  cerr << m.v[3]->z.real() << endl;
  m.show(); m.pause();
}
