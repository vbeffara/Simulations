/*
 * SLE - Schramm's Stochastic Loewner Evolution
 */

#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace vb;

#define DONTKNOW  Color(0)
#define INSIDE    Color(0)
#define LEFTSIDE  Color(255,0,0)
#define RIGHTSIDE Color(0,255,0)

int n,nn,wd,ht,jmax;
Image *img;
double *c,*cmax,*cmin;
double kappa;

Color color(int x, int y)
{
  double a,b,d;
  int k;
  
  a=x-wd; b=y; d=0;
  
  for (k=0;(k<n)&&(b>0)&&(a>=cmin[k])&&(a<=cmax[k]);k++) {
    d=kappa / ((a-c[k])*(a-c[k]) + b*b);
    a += (a-c[k])*d;
    b -= b*d;
  }

  if (k>0) --k;

  if (a<c[k]) return LEFTSIDE;
  else return RIGHTSIDE;
}

Color color_smooth(int x, int y)
{
  double a,b,d;
  int k;

  a=x-wd; b=y; d=0;

  for (k=0;(k<n)&&(b>0);k++) {
    d=kappa / ((a-c[k])*(a-c[k]) + b*b);
    a += (a-c[k])*d;
    b -= b*d;
  }

  a -= c[k-1];
  if (a<-b) return 1;
  if (a>b) return 255;
  
  double t = (a+b)/(2*b);
  t = t*t*(3-2*t);

  return Color (1 + 254*t);
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
  
  /* lecture des arguments */

  CL_Parser CLP (argc,argv,"n=300,k=2.666666666667,r=0,s",
      "Syntax: SLE [-n size] [-k kappa] [-r random_seed] [-s]");
  nn = CLP('n');
  kappa = CLP('k');
  int r = CLP('r');
  int smooth = CLP('s');

  sprintf(s,"Schramm's SLE Process (kappa=%f)",kappa);

  PRNG prng;
  if (r) prng.srand(r);

  n=nn*nn; kappa=2/kappa;

  /* Brownien qui conduit le SLE - kappa n'apparait pas ici*/
  
  c = new double[n];
  c[0]=0; cd=0;
  for (i=1;i<n;i++) {
    d = prng.gaussian();
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
  
  img = new Image (2*wd,ht,(smooth?8:2),s);

  for (i=0;i<2*wd;i++) img->putpoint (i,0, (smooth?255:INSIDE));

  img->show ();

  /*  C'est parti */

  img->tessellate ((smooth?color_smooth:color),0,1,2*wd-1,ht-1);

  if (smooth) {
    for (int x=0; x<2*wd; ++x) {
      for (int y=1; y<ht; ++y) {
        if ((*img)(x,y)==1)
          img->putpoint(x,y,0);
      }
    }
  } else edge_detect();

  img->output_png();
  delete img;
  return 0;
}
