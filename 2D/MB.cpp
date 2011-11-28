/*
 * Simulation d'une marche simple dans Z^2, tu?e ? la sortie d'un
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
  int n;                   /* Taille du carré */
  char d;                  /* La chaîne */
  
  long i,x,y,done;

  /* arguments -> taille du terrain */

  CL_Parser CLP (argc,argv,"n=500");
  n = CLP('n');

  /* Initialisations */

  img = new Image(2*n,2*n, str(fmt("A Brownian path of size %d")%n));
  for (int i=0; i<2*n; ++i)
    for (int j=0; j<2*n; ++j)
      img->putpoint(i,j,Color(253,246,227));

  /* Simulation */

  i=0; done=0; x=n; y=n;
  while (!done) {
    d = prng()&3;
    img->putpoint (x,y,Color(88,110,117));
    x += dx[(int)d];
    y += dy[(int)d];
    /*  fprintf(stderr,"%8ld\r",++i); */

    if ((x==2*n-1)||(y==2*n-1)||(x==0)||(y==0)) done=1;
  }

  img->output();

  return 0;
}
