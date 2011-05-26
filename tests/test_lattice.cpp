
#include <vb/Lattice.h>
#include <vb/PRNG.h>
#include <vb/Figure.h>

using namespace vb;
using namespace std;

int main (int argc, char ** argv) {

  cpx tau (.5, sqrt(3.0)/2);

  // The square lattice

  Lattice Z2(1,cpx(0,1));

  Z2.bond (0,0,1,0);
  Z2.bond (0,0,0,1);

  Z2.z[0]=0;

  // The hexagonal lattice:

  Lattice H(2,tau);

  H.bond (0,1);
  H.bond (0,1,-1,0);
  H.bond (0,1,0,-1);

  H.z[0]=0;
  H.z[1]=(cpx(1)+tau)/cpx(3);

  // The hexagonal lattice with a rectangular fundamental domain:

  Lattice H2(4,cpx(0,sqrt(3.0)));

  H2.bond (0,1);
  H2.bond (1,2);
  H2.bond (2,3);
  H2.bond (3,0,0,1);
  H2.bond (2,1,1,0);
  H2.bond (3,0,1,1);

  H2.z[0]=0;
  H2.z[1]=cpx(0,sqrt(3)/3);
  H2.z[2]=cpx(.5,sqrt(3)/2);
  H2.z[3]=cpx(.5,5*sqrt(3)/6);

  // The triangular lattice:

  Lattice T(1,tau);

  T.bond (0,0,1,0);
  T.bond (0,0,0,1);
  T.bond (0,0,-1,1);

  T.z[0]=0;

  // Triangular lattice with a rectangular fundamental domain:

  Lattice T2(2,cpx(0,sqrt(3.0)));

  T2.bond (0,1);
  T2.bond (0,0,1,0);
  T2.bond (1,0,1,0);
  T2.bond (1,0,0,1);
  T2.bond (1,0,1,1);
  T2.bond (1,1,1,0);

  T2.z[0]=0;
  T2.z[1]=cpx(.5,sqrt(3.0)/2);

  // The Kagome lattice

  Lattice K(3,tau);

  K.bond (0,1);
  K.bond (0,2);
  K.bond (1,2);
  K.bond (1,0,1,0);
  K.bond (2,0,0,1);
  K.bond (1,2,1,-1);

  K.z[0]=0;
  K.z[1]=.5;
  K.z[2]=tau/cpx(2);

  // The Kagome lattice with a rectangular fundamental domain:

  Lattice K2(6,cpx(0,sqrt(3)));

  K2.bond (0,1);
  K2.bond (0,2);
  K2.bond (1,2);
  K2.bond (2,3);
  K2.bond (2,4);
  K2.bond (3,4);
  K2.bond (4,5);
  K2.bond (1,0,1,0);
  K2.bond (4,3,1,0);
  K2.bond (5,3,1,0);
  K2.bond (5,1,0,1);
  K2.bond (5,0,1,1);

  K2.z[0]=0;
  K2.z[1]=.5;
  K2.z[2]=cpx(.25,sqrt(3)/4);
  K2.z[3]=cpx(0,sqrt(3)/2);
  K2.z[4]=cpx(.5,sqrt(3)/2);
  K2.z[5]=cpx(.75,3*sqrt(3)/4);

  // The square-octogon lattice

  Lattice SO(4,cpx(0,1));

  SO.bond (0,1);
  SO.bond (1,2);
  SO.bond (2,3);
  SO.bond (3,0);
  SO.bond (2,0,0,1);
  SO.bond (1,3,1,0);

  SO.z[0]=cpx(.5,.25);
  SO.z[1]=cpx(.75,.5);
  SO.z[2]=cpx(.5,.75);
  SO.z[3]=cpx(.25,.5);

  // Random stuff on a chosen lattice:

  Lattice &L = K2;

  Lattice_rectangle<Shape*> R(L,9,5);
  Pen p (Color(255,0,0),1,Color(255,255,0));

  for (int i=0; i<R.w; ++i)
    for (int j=0; j<R.h; ++j)
      for (int k=0; k<L.n; ++k)
        R(i,j,k) = new Circle (Lattice_vertex(L,i,j,k), prng.uniform_real(0,0.3), p);

  // Make a figure out of it

  Figure F;

  for (int i=0; i<R.w; ++i)
    for (int j=0; j<R.h; ++j)
      for (int k=0; k<L.n; ++k)
        for (int l=0; l<L.adj[k].size(); ++l)
          F.add (new Segment (Lattice_vertex(L,i,j,k), Lattice_vertex(L,i,j,k) + L.adj[k][l]));

  for (int i=0; i<R.w; ++i)
    for (int j=0; j<R.h; ++j)
      for (int k=0; k<L.n; ++k)
        F.add (R(i,j,k));

  F.show(); F.pause();
  return 0;
}
