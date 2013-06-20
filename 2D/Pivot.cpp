
#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace vb;

int n,nn;
Image *img;
int *x,*y;

inline int COO(int i, int j)
{
  int k;

  if (i<-nn) i=-nn;
  if (i>=nn) i=nn-1;
  if (j<-nn) j=-nn;
  if (j>=nn) j=nn-1;

  k=2*nn*(j+nn)+(i+nn);
  return k;
}

inline int RCOO(int i, int j, int ii, int jj)
{
  int k,l;

  k = ii-(j-jj);
  l = jj+(i-ii);
  return COO(k,l);
}

int main(int argc, char ** argv)
{
  int i,j,k,l,cont,piv,p;

  // Initialisations

  if (argc != 2) {
    fprintf(stderr, "Syntaxe : Pivot <sqrt(n)>\n");
    exit(1);
  }
  sscanf (argv[1],"%d",&nn); nn=(nn>>3)<<3; n=nn*nn;

  img = new Image (2*nn,2*nn,"A pivoting point of angle pi/2");

  x = new int[n];
  y = new int[n];

  // Brownien

  cont=1;
  while(cont) {
    x[0]=0; y[0]=0; i=0; j=0;
    for(k=1;k<n;k++) {
      if (prng()&16) {
        i += (prng()&64?1:-1);
      } else {
        j += (prng()&4?1:-1);
      }
      x[k]=i; y[k]=j;
    }

    // Recherche d'un pivot

    for (k=0; k<4*n; k++)  img->put (coo(k,0),0);
    for (k=0; k<n>>2; k++) img->put (coo(COO(x[k],y[k]),0),0);
    p=0;
    for (k=n>>2; (k<(n>>2)+(n>>1)) && cont; k++) {
      piv=1;
      for (l=k; (l<n) && piv; l++) {
        if ((*img).at(coo(COO(x[l],y[l]),0)) || (*img).at(coo(RCOO(x[l],y[l],x[k],y[k]),0)))
          piv=0;
      }
      if (piv) {
        img->put (coo(COO(x[k],y[k]),0),2);
        p=k; cont=0;
      } else if ((*img).at(coo(COO(x[k],y[k]),0))==Color(0)) img->put(coo(COO(x[k],y[k]),0),1);
    }
    if (cont) fprintf(stderr, ".");
  }

  fprintf(stderr, "\nPivot found at time %d (%d,%d).\n", p, x[p], y[p]);

  for (k=0; k<4*n; k++) img->put(coo(k,0),0);
  for (k=0; k<n; k++)   img->put(coo(COO(x[k]-x[p],y[k]-y[p]),0),3);
  for (k=p+1; k<n; k++) {
    if ((*img).at(coo(RCOO(x[k]-x[p],y[k]-y[p],0,0),0)) != Color(3)) {
      img->put(coo(RCOO(x[k]-x[p],y[k]-y[p],0,0),0),1);
    }
  }

  return 0;
}
