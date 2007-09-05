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

  C4.add_edge(2,0,PG_HERE);
  C4.add_edge(2,1,PG_HERE);
  C4.add_edge(2,0,PG_NORTH);

  cout << setprecision(10);

  cout << C4 << endl;
  cout << "Shear: " << C4.rw_shear() << endl;

  cout << "-- Relaxing to the square embedding --" << endl;
  C4.relax();
  cout << C4 << endl;
  cout << "Shear: " << C4.rw_shear() << endl;

  cout << "-- Setting modulus to the RW embedding --" << endl;
  C4.rw_tau();
  cout << C4 << endl;
  cout << "Shear: " << C4.rw_shear() << endl;
}
