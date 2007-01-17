/*
 * Vertex-Once-Reinforced Random Walk in Z^2
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <vb.h>

int dx[4] = {1,0,-1,0};  /* 0=est, 1=nord ... */
int dy[4] = {0,1,0,-1};

using namespace vb;

int main(int argc, char ** argv)
{
  char title[80];          /* Titre ... */
  int n;                   /* Taille du carré */
  char d;                  /* La chaine */
  double a;                /* Renforcement a 1+a */

  int L;                /* Coarse grain, right now it is n^(1/3) */
  
  long x,y,nx,ny;

  char newcol;

  /* arguments -> taille du terrain */

  if (argc != 3) {
    fprintf(stderr, "Syntaxe : Once <n> <a>\n");
    exit(1);
  }
  n=atoi(argv[1]);
  a=atof(argv[2]);
  a=1/(1+a);

  /* Initialisations */

  srand48(time(0));

  sprintf(title,"A vertex once-reinforced random walk (size=%d, a=%.2f)",n,1/a-1);

  L = (int) pow((double)n,1./3.);
  fprintf (stderr, "Coarse graining with mesh %d.\n", L);

  CoarseImage img(n,n,L,title);
  img.outputsize = 15;

  img.onscreen();

  /* Simulation */

  x=n/2; y=n/2;

  while ((x>0)&&(y>0)&&(x<n-1)&&(y<n-1)) {
    img.putpoint (x,y,1);

    d = (lrand48()>>10)&3;
    nx = x + dx[(int)d];
    ny = y + dy[(int)d];
    newcol = img(nx,ny);

    if (newcol||(drand48()<a)) {
       x=nx; y=ny;
    }
  }

  std::cout << img;
  return 0;
}
