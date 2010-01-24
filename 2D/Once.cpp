/*
 * Vertex-Once-Reinforced Random Walk in Z^2
 */

#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/CoarseImage.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int dx[4] = {1,0,-1,0};  /* 0=est, 1=nord ... */
int dy[4] = {0,1,0,-1};

using namespace vb;

int main(int argc, char ** argv)
{
  char d;                  /* La chaine */

  int L;                /* Coarse grain, right now it is n^(1/3) */
  
  long x,y,nx,ny;

  char newcol;

  /* arguments -> taille du terrain */

  CL_Parser CLP (argc,argv,"n=1000,a=2");
  int n = CLP('n');
  double a = CLP('a');
  a=1/(1+a);

  /* Initialisations */

  L = (int) pow((double)n,1./3.);
  std::cerr << "Coarse graining with mesh " << L << ".\n";

  CoarseImage img(n,n,L,fmt("A vertex once-reinforced random walk (size=%, a=%)") % n % (1/a-1));
  //img.snapshot_period = t;

  img.show();

  /* Simulation */

  x=n/2; y=n/2;

  while ((x>0)&&(y>0)&&(x<n-1)&&(y<n-1)) {
    img.putpoint (x,y,1);

    d = prng.rand()&3;
    nx = x + dx[(int)d];
    ny = y + dy[(int)d];
    newcol = img(nx,ny);

    if (newcol || prng.bernoulli(a)) {
       x=nx; y=ny;
    }
  }

  return 0;
}
