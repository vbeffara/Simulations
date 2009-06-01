/*
 * Vertex-Once-Reinforced Random Walk in Z^2
 */

#include <vb.h>
#include <vb/CoarseImage.h>

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int dx[4] = {1,0,-1,0};  /* 0=est, 1=nord ... */
int dy[4] = {0,1,0,-1};

using namespace vb;

int main(int argc, char ** argv)
{
  char title[80];          /* Titre ... */
  char d;                  /* La chaine */

  int L;                /* Coarse grain, right now it is n^(1/3) */
  
  long x,y,nx,ny;

  char newcol;

  /* arguments -> taille du terrain */

  CL_Parser CLP (argc,argv,"n=1000,a=2");
  int n = CLP.as_int('n');
  double a = CLP('a');
  a=1/(1+a);

  /* Initialisations */

  PRNG prng;

  sprintf(title,"A vertex once-reinforced random walk (size=%d, a=%.2f)",n,1/a-1);

  L = (int) pow((double)n,1./3.);
  fprintf (stderr, "Coarse graining with mesh %d.\n", L);

  CoarseImage img(n,n,L,title);
  img.outputsize = 15;
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

  std::cout << img;
  return 0;
}
