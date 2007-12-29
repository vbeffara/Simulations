#include <iostream>
#include <iomanip>
#include <vb/Map.h>
#include <fltk/run.h>

using namespace std;
using namespace vb;

template <typename T1, typename T2> ostream &operator<< (ostream &os, pair<T1,T2> p) {
  os << '(' << p.first << ',' << p.second << ')';
  return os;
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

  m.barycentric();
  m.barycentric();
  m.barycentric();
  m.barycentric();

  m.inscribe(m.face(Edge(0,m.v[0].adj.back())));

  Figure F;
  for (int i=0; i<m.n; ++i) m.v[i].r=.5/sqrt((double)m.n);
  m.plot_circles(F);

  F.show();
  m.balance(&F);
  cout << "Done." << endl;
  fltk::run();
  return 0;
}
