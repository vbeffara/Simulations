
#include <vb/Lattice.h>
#include <vb/PRNG.h>
#include <vb/Figure.h>

using namespace vb;
using namespace std;

int main (int argc, char ** argv) {
  Lattice L = SO();
  L.relax(1e-14);
  L.tau = L.tau_rw();
  L.optimize (cost_cp);

  Lattice_rectangle<int> R(L,3,3);
  Pen p (Color(255,0,0),1,Color(255,255,0));

  // Make a figure out of it

  Figure F;

  vector<cpx> fd;
  fd.push_back(L(1,1));
  fd.push_back(L(2,1));
  fd.push_back(L(2,2));
  fd.push_back(L(1,2));
  F.add (new Polygon(fd, Pen(0,0,200,1)));

  for (int i=0; i<R.w; ++i)
    for (int j=0; j<R.h; ++j)
      for (int k=0; k<L.n; ++k)
        F.add (new Circle (L(i,j,k), L.r[k], Pen(0,.2)));

  for (int i=0; i<R.w; ++i)
    for (int j=0; j<R.h; ++j)
      for (int k=0; k<L.n; ++k)
        for (int l=0; l<L.adj[k].size(); ++l)
          F.add (new Segment (L(i,j,k), L(i,j,k) + L.shift(k,l), Pen(0,.1)));

  F.show(); F.pause(); F.output_pdf();
  return 0;
}
