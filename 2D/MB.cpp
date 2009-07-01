/*
 * Simulation d'une marche simple dans Z^2, tuée à la sortie d'un
 * carré de côté donné. 
 */

#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

#define EC fprintf(stderr,"<%d>\n",__LINE__);

using namespace vb;

int dx[4] = {1,0,-1,0};  /* 0=est, 1=nord ... */
int dy[4] = {0,1,0,-1};

int main(int argc, char ** argv)
{
  Image *img;              /* L image */
  char title[80];          /* Titre ... */
  int n;                   /* Taille du carré */
  char d;                  /* La chaine */
  
  long i,x,y,done;

  /* arguments -> taille du terrain */

  CL_Parser CLP (argc,argv,"n=500");
  n = CLP('n');

  /* Initialisations */

  PRNG prng;

  sprintf(title,"A Brownian path of size %d",n);
  img = new Image(2*n,2*n,1,title);

  /* Simulation */

  i=0; done=0; x=n; y=n;
  while (!done) {
    d = prng.rand()&3;
    img->putpoint (x,y,1);
    x += dx[(int)d];
    y += dy[(int)d];
    /*  fprintf(stderr,"%8ld\r",++i); */

    if ((x==2*n-1)||(y==2*n-1)||(x==0)||(y==0)) done=1;
  }

  img->cropped = 1;
  std::cout << *img;
  return 0;
}
