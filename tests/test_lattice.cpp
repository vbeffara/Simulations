
#include <vb/Lattice.h>
#include <vb/PRNG.h>

using namespace vb;
using namespace std;

int main (int argc, char ** argv) {

  // The hexagonal lattice:

  cpx tau (.5, sqrt(3.0)/2);

  Lattice H(2,tau);
  H.bond (0,1);
  H.bond (0,1,-1,0);
  H.bond (0,1,0,-1);

  H.z[0]=0;
  H.z[1]=(cpx(1)+tau)/cpx(3);

  // Random walk on the hexagonal lattice:

  Lattice_vertex P(H);

  for (int i=0; i<10000; ++i) {
    cpx z=P.z();
    cout << z.real() << " " << z.imag() << endl;
    P += H.adj[P.k][prng.uniform_int(3)];
  }

  return 0;
}
