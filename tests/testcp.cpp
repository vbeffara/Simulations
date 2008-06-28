
#include <vb/Minimizer.h>
#include <vb/Map.h>
#include <iostream>

using namespace vb;
using namespace std;

int center = 6;
int infinity = 13;

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
  m.barycentric();
  m.barycentric();
  m.inscribe(m.face(Edge(1,m.v[1]->adj.back())));

  m.balance();

  m.show();
  m.pause();

  Vector<double> x(3*m.n);

  double r = 1.0/sqrt((double)m.n);

  for (int i=0; i<m.n; ++i) {
    x[3*i]        = m.v[i]->z.real() / (1-r);
    x[3*i+1]      = m.v[i]->z.imag() / (1-r);
    x[3*i+2]      = .8*r;
  }

  Minimizer<double> MM (3*m.n, Map_fg_circle_disk, &m);
  MM.os = &cerr;

  MM.minimize_qn (x);
  x = MM.x;

  std::cerr << "Number of vertices:    " << m.n << std::endl;
  std::cerr << "Final value of f:      " << MM.fx << std::endl;
  std::cerr << "Final square gradient: " << scalar_product(MM.gx,MM.gx) << std::endl;

  for (int i=0; i<m.n; ++i) {
    m.v[i]->z = cpx (x[3*i], x[3*i+1]);
    m.v[i]->r = x[3*i+2];
  }

  Figure f;
  m.plot_circles(f);
  f.circle(cpx(0.0,0.0),1.0);
  f.show();
  f.pause();
  f.printASY ("cp.asy");

  return 0;
}
