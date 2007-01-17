
#include <math.h>
#include <time.h>
#include <vb.h>

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

  img = new Image (2*nn,2*nn,2,"A pivoting point of angle pi/2");

  x = new int[n];
  y = new int[n];

  // Brownien

  cont=1;
  while(cont) {
    srand(time(0));
    x[0]=0; y[0]=0; i=0; j=0;
    for(k=1;k<n;k++) {
      if (rand()&16) {
	i += (rand()&64?1:-1);
      } else {
	j += (rand()&4?1:-1);
      }
      x[k]=i; y[k]=j;
    }
    
    // Recherche d'un pivot
    
    for (k=0; k<4*n; k++) (*img)(k)=0;
    for (k=0; k<n>>2; k++) (*img)(COO(x[k],y[k]))=1;
    p=0;
    for (k=n>>2; (k<(n>>2)+(n>>1)) && cont; k++) {
      piv=1;
      for (l=k; (l<n) && piv; l++) {
	if ((*img)(COO(x[l],y[l])) || (*img)(RCOO(x[l],y[l],x[k],y[k])))
	  piv=0;
      }
      if (piv) {
	(*img)(COO(x[k],y[k]))=2;
	p=k; cont=0;
      } else if ((*img)(COO(x[k],y[k]))==0) (*img)(COO(x[k],y[k]))=1;
    }
    if (cont) fprintf(stderr, ".");
  }
  
  fprintf(stderr, "\nPivot found at time %d (%d,%d).\n", p, x[p], y[p]);

  for (k=0; k<4*n; k++) (*img)(k)=0;
  for (k=0; k<n; k++) (*img)(COO(x[k]-x[p],y[k]-y[p]))=3;
  for (k=p+1; k<n; k++) {
    if ((*img)(RCOO(x[k]-x[p],y[k]-y[p],0,0)) != 3) {
      (*img)(RCOO(x[k]-x[p],y[k]-y[p],0,0))=1;
    }
  }

  img->cropped = 1;
  std::cout << *img;

  return 0;
}    
