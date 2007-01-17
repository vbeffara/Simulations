/*
 * SLE - Schramm's Stochastic Loewner Evolution
 */

#include <math.h>
#include <time.h>
#include <vb.h>
#include <unistd.h>

using namespace vb;

#define DONTKNOW  0
#define INSIDE    3
#define LEFTSIDE  1
#define RIGHTSIDE 2

int n,nn,wd,ht,jmax;
Image *img;
double *c,*cmax,*cmin;
double kappa;

char color(int x, int y)
{
  double a,b,d;
  int k;
  
  a=x-wd; b=y; d=0;
  
  for (k=0;(k<n)&&(b>0)&&(a>=cmin[k])&&(a<=cmax[k]);k++) {
    d=kappa / ((a-c[k])*(a-c[k]) + b*b);
    a += (a-c[k])*d;
    b -= b*d;
  }
  if (a<c[k-1]) return LEFTSIDE;
  else return RIGHTSIDE;
}

void edge_detect(void)
{
  int x,y;
  char self,right,down;
  
  for (y=1;y<ht;y++) {
    for (x=-wd;x<wd-1;x++) {
      self = (*img)(wd+x,y);
      if (x<nn-1) { 
	right = (*img)(wd+x+1,y);
      } else {
	right = self;
      }
      if (y<ht-1) {
	down = (*img)(wd+x,y+1);
      } else {
	down = self;
      }

      if ((self!=right)||(self!=down)) {
	img->putpoint (x+wd,y,INSIDE);
      }
    }
  }
}

int main(int argc, char ** argv)
{
  int i; 
  char s[80];
  double d, cd;
  char ch;
  
  /* lecture des arguments */

  n = time(0);
  nn = 300;
  kappa = 8.0/3.0;

  while ((ch = getopt(argc,argv,"n:k:r:h")) != -1) {
    switch (ch) {
    case 'n':
      nn = atoi (optarg);
      break;
    case 'k':
      kappa = atof (optarg);
      break;
    case 'r':
      n = atoi (optarg);
      break;
    case 'h':
      fprintf (stderr, "Syntax: %s [-n size] [-k kappa] [-r random_seed]\n", argv[0]);
      exit (1);
      break;
    }
  }

  sprintf(s,"Schramm's SLE Process (kappa=%f)",kappa);
  srand48(n);
  n=nn*nn; kappa=2/kappa;

  /* Brownien qui conduit le SLE - kappa n'apparait pas ici*/
  
  c = new double[n];
  c[0]=0; cd=0;
  for (i=1;i<n;i++) {
    d = 2*sqrt(3.0)*drand48() - sqrt(3.0); /*  E=0, Var=1 */
    c[i] = c[i-1] + d;
  }

  /*  Pont brownien */

  d = c[n-1]/((double)n);
  for (i=1; i<n;i++) { c[i] -= i*d; }

  /*  Optimisations : si on sort du support on survit */

  cmin = new double[n+1];
  cmax = new double[n+1];
  cmax[n]=c[n-1]; cmin[n]=c[n-1];
  for (i=n-1;i>0;i--) {
    cmin[i] = (c[i-1]<cmin[i+1])?c[i-1]:cmin[i+1];
    cmax[i] = (c[i-1]>cmax[i+1])?c[i-1]:cmax[i+1];
  }
  cmin[0]=cmin[1]; cmax[0]=cmax[1];

  wd = (int)(-cmin[0]>cmax[0]?-cmin[0]:cmax[0]);
  ht = (int) ( 2*n*sqrt(kappa)/(cmax[0]-cmin[0]) ); /*  TODO : Fix */

  /* Image */
  
  img = new Image (2*wd,ht,2,s);

  for (i=0;i<2*wd;i++) (*img)(i) = INSIDE;

  img->onscreen ();

  /*  C'est parti */

  fprintf (stderr, "Doing the hard work ...\n");
  
  img->tesselate (color,0,1,2*wd-1,ht-1);

  fprintf (stderr, "Good bye, have a nice day.\n");
  
  edge_detect();

  std::cout << (*img);

  return 0;
}
