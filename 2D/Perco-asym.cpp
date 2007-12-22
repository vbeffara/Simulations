
#include <math.h>
#include <vb.h>

using namespace vb;

char *right, *down;
Image *img;
long n;
long pivv,pivh;

void findpiv (long hcross) 
{
  int x,y;

  pivv=0; pivh=0;
  if (hcross==0) {
    for (x=0;x<n-1;x++)
      for (y=0;y<n;y++)
	if ((*img)(x+n*y)*(*img)(x+1+n*y)==2)
	  pivh++;
    for (x=0;x<n;x++)
      for (y=0;y<n-1;y++)
	if ((*img)(x+n*y)*(*img)(x+n+n*y)==2)
	  pivv++;
  } else {
    for (x=0;x<n-1;x++)
      for (y=0;y<n;y++)
	if ((*img)(x+n*y)*(*img)(x+1+n*y)==9) {
	  pivh++;
	}
    for (x=0;x<n;x++)
      for (y=0;y<n-1;y++)
	if ((*img)(x+n*y)*(*img)(x+n+n*y)==9) {
	  pivv++;
	}
  }
}

void explore (long x0, long y0, long d0, long xf, long yf, char c)
{
  long x,y,xy,d;

  x=x0; y=y0; xy=x+n*y; d=d0;

  while ((x!=xf)||(y!=yf)) {
    switch (d) {
    case 0: /* Right */
      if ((x<n-1)&&(right[xy])) {
	x++; xy++; d=3;
	img->putpoint (x,y,(c|(*img)(xy)));
      } else {
	d=1;
      }
      break;
    case 1: /* Top */
      if ((y<n-1)&&(down[xy+n])) {
	y++; xy+=n; d=0;
	img->putpoint (x,y,(c|(*img)(xy)));
      } else {
	d=2;
      }
      break;
    case 2: /* Left */
      if ((x>0)&&(right[xy-1])) {
	x--; xy--; d=1;
	img->putpoint (x,y,(c|(*img)(xy)));
      } else {
	d=3;
      }
      break;
    case 3: /* Down */
      if ((y>0)&&(down[xy])) {
	y--; xy-=n; d=2;
	img->putpoint (x,y,(c|(*img)(xy)));
      } else {
	d=0;
      }
      break;
    }
  }
} 

int main(int argc, char ** argv)
{
  long i,j,hcross;
  char title[80];
  double epsilon;

  /* arguments -> p et n */

  if (argc != 3) {
    fprintf(stderr, "Syntaxe : Perco <n> <epsilon>\n");
    exit(1);
  }
  n=atoi(argv[1]);
  epsilon=atof(argv[2]);

  PRNG prng;

  /* Percolation ... */

  right = new char[n*n];
  down = new char[n*n];

  for (i=0;i<n*n;i++) {
    if (prng.bernoulli(.5+epsilon)) right[i]=1;
    if (prng.bernoulli(.5-epsilon)) down[i]=1;
  }

  sprintf(title,"Asymmetric bond-percolation cluster (epsilon=%f)",epsilon);
  img = new Image(n,n,2,title);

  /* OnScreen (img); */

  for (i=0;i<n;i++) { 
    down[n*i]=1; 
    down[n*i+n-1]=0;
    right[i]=0;
    right[(n-1)*n+i]=0;
  }
  explore (0,0,0,0,n-1,1);

  hcross=0;
  for (i=0;i<n;i++)
    if ((*img)((n-1)+n*i)==1)
      hcross=1;
  
  if (hcross==0) {
    for (i=0;i<n;i++) { 
      down[n*i+n-1]=1;
    }
    explore (n-1,n-1,2,n-1,0,2);
  } else {
    for (i=0;i<n;i++) 
      for (j=0;j<n;j++)
	img->putpoint (i,j,0);

    for (i=0;i<n;i++) { 
      down[n*i+n-1]=1;
    }
    explore (0,n-1,3,n-1,n-1,1);
    explore (n-1,0,1,0,0,2);
  }
  
  for (i=0;i<n;i++) {
    img->putpoint (0,i,0);
    img->putpoint (n-1,i,0);
  }

  findpiv(hcross);

  /* If there is no horiz crossing, can count the pivotal edges */

  if (hcross==1) {
    printf ("%5ld %5ld (h/v for a horizontal crossing)\n", pivh, pivv);
  } else {
    printf ("%5ld %5ld (h/v for a vertical crossing)\n", pivh, pivv);
  }

  return 0;
}
