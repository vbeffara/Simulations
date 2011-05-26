
#include <vb/Lattice.h>
#include <vb/PRNG.h>
#include <vb/Figure.h>

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

  // Random stuff on a hexagonal lattice:

  Lattice_rectangle<double> R(H,10,10);

  for (int i=0; i<R.w; ++i)
    for (int j=0; j<R.h; ++j)
      for (int k=0; k<2; ++k)
        R(i,j,k) = prng.uniform_real(0,.5);

  // Make a figure out of it

  Figure F;
  Pen p (Color(255,0,0),1,Color(255,255,0));

  for (int i=0; i<R.w; ++i)
    for (int j=0; j<R.h; ++j)
      for (int k=0; k<2; ++k) {
        for (int l=0; l<H.adj[k].size(); ++l)
          F.add (new Segment (Lattice_vertex(H,i,j,k), Lattice_vertex(H,i,j,k) + H.adj[k][l]));
        F.add (new Circle (Lattice_vertex(H,i,j,k), R(i,j,k), p));
      }

  F.show(); F.pause();
  return 0;
}
