#include <iostream>
#include <vb/Map.h>

using namespace std;
using namespace vb;

template <typename T1, typename T2> ostream &operator<< (ostream &os, pair<T1,T2> p) {
  os << '(' << p.first << ',' << p.second << ')';
  return os;
}

int main () {
  Map m (4);

  m << Edge(0,1) << Edge(0,3) << Edge(0,2)
    << Edge(1,2) << Edge(1,3) << Edge(1,0)
    << Edge(2,0) << Edge(2,3) << Edge(2,1)
    << Edge(3,0) << Edge(3,1) << Edge(3,2);

  cout << m;
  return 0;
}
