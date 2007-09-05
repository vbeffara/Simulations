//
// Test of vb/PerioGraph.h
//

#include <iostream>
#include <vb/PerioGraph.h>

int main (int argc, char **argv) {
  //
  // This is the cell for the triangular lattice
  //

  vb::PerioCell C(2);

  C.add_edge(0,0,PG_EAST);
  C.add_edge(1,1,PG_EAST);
  C.add_edge(0,1,PG_HERE);
  C.add_edge(0,1,PG_WEST);
  C.add_edge(0,1,PG_SOUTHWEST);
  C.add_edge(0,1,PG_SOUTH);

  std::cout << C << std::endl;
}
