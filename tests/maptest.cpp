#include <iostream>
#include <vb/Map.h>

using namespace std;
using namespace vb;

template <typename T1, typename T2> ostream &operator<< (ostream &os, pair<T1,T2> p) {
  os << '(' << p.first << ',' << p.second << ')';
  return os;
}

int main () {
  Map m (5);

  m << Edge(0,1) << Edge(0,2) << Edge(0,3) << Edge(0,4)
    << Edge(1,2) << Edge(1,0) << Edge(1,4)
    << Edge(2,3) << Edge(2,0) << Edge(2,1)
    << Edge(3,4) << Edge(3,0) << Edge(3,2)
    << Edge(4,1) << Edge(4,0) << Edge(4,3);

  m.print_as_dot (cout);

  return 0;
}
