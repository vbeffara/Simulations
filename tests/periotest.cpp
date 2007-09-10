//
// Test of vb/PerioGraph.h
//

#include <iostream>
#include <iomanip>
#include <vb/PerioGraph.h>

using namespace std;

int main (int argc, char **argv) {
  //
  // This is the cell for the triangular lattice
  //
  // +-+
  // |/|
  // +-+

  vb::PerioCell C(2);

  C.add_edge(0,0,PG_EAST);
  C.add_edge(1,1,PG_EAST);
  C.add_edge(0,1,PG_HERE);
  C.add_edge(0,1,PG_WEST);
  C.add_edge(0,1,PG_SOUTHWEST);
  C.add_edge(0,1,PG_SOUTH);

  //
  // This is the centered square lattice
  //
  // +-+
  // |X|
  // +-+

  vb::PerioCell C4(3);

  C4.add_edge(0,0,PG_NORTH);
  C4.add_edge(0,0,PG_SOUTH);
  C4.add_edge(0,0,PG_EAST);
  C4.add_edge(0,0,PG_WEST);
  C4.add_edge(0,1,PG_HERE);
  C4.add_edge(0,1,PG_WEST);
  C4.add_edge(0,1,PG_SOUTHWEST);
  C4.add_edge(0,1,PG_SOUTH);

  //
  // Refined:
  //
  // +---+
  // |\ /|
  // |}X |
  // |/ \|
  // +---+

  C4.add_edge(2,0,PG_HERE);
  C4.add_edge(2,1,PG_HERE);
  C4.add_edge(2,0,PG_NORTH);

  //
  // This is a less symmetric graph
  //
  // +-+-+
  // |/|\|
  // +-+-+
  // |/|/|
  // +-+-+

  vb::PerioCell C7(4);

  C7.add_edge(0,1,PG_HERE);
  C7.add_edge(0,2,PG_HERE);
  C7.add_edge(0,3,PG_HERE);
  C7.add_edge(1,2,PG_HERE);
  C7.add_edge(2,3,PG_HERE);
  C7.add_edge(1,0,PG_EAST);
  C7.add_edge(1,3,PG_EAST);
  C7.add_edge(2,3,PG_EAST);
  C7.add_edge(2,1,PG_NORTH);
  C7.add_edge(3,0,PG_NORTH);
  C7.add_edge(3,1,PG_NORTH);
  C7.add_edge(3,1,PG_NORTHWEST);

  //
  // No symmetry ?
  //

  vb::PerioCell C5(5);

  C5.add_edge(0,1,PG_HERE);
  C5.add_edge(0,4,PG_HERE);
  C5.add_edge(1,2,PG_HERE);
  C5.add_edge(1,3,PG_HERE);
  C5.add_edge(1,4,PG_HERE);
  C5.add_edge(2,3,PG_HERE);
  C5.add_edge(3,4,PG_HERE);
  C5.add_edge(2,0,PG_EAST);
  C5.add_edge(2,4,PG_EAST);
  C5.add_edge(3,4,PG_EAST);
  C5.add_edge(3,0,PG_NORTHEAST);
  C5.add_edge(3,1,PG_NORTHEAST);
  C5.add_edge(3,2,PG_NORTH);
  C5.add_edge(3,1,PG_NORTH);
  C5.add_edge(4,1,PG_NORTH);

  C5.relax();
  cerr << setprecision(20);
  cerr << C5.cp_tau() << endl;
}
