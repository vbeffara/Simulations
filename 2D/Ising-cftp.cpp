
#include <vb.h>
#include <math.h>
#include <cstdio>

using namespace vb;

char *high, *low;
long n;
long p[3];

PRNG_Rewindable prng;

void one_step (void) {
  int x,y,xy,sgn,delta;
  long rdm;

  for (x=1;x<n-1;x++) {
    xy=x;
    for (y=1;y<n-1;y++) {
      sgn = (prng.rand() & 32) / 32;
      rdm = prng.rand();
      
      xy += n;
      
      delta = 2-(low[xy+1] + low[xy-1] + low[xy+n] + low[xy-n]);
      if (!sgn) delta = -delta;
      
      if ( (delta<=0) || ( rdm < p[delta] ) )
	{ low[xy]=sgn; }
      
      delta = 2-(high[xy+1] + high[xy-1] + high[xy+n] + high[xy-n]);
      if (!sgn) delta = -delta;
      
      if ( (delta<=0) || ( rdm < p[delta] ) )
	{ high[xy]=sgn; }      
    }
  }
}

long paint (Image &img) {
  int x,y,xy;
  long dist=0;
  
  for (x=0;x<n;x++) {
    for (y=0;y<n;y++) {
      xy = x + n*y;
      if (low[xy]==high[xy])
	img.putpoint (x,y,3*low[xy],0);
      else {
	img.putpoint (x,y,2,0);
	dist++; 
      }
    }
  }
  img.update();
  return dist;
}

int main(int argc, char *argv[])
{
  long i,j,bla;
  char title[80];
  long dist;

  /* arguments -> beta et n
   * beta_critical = log(1+sqrt(2)) = 0.88137359...
   */

  CL_Parser CLP (argc,argv,"b=.88137359,n=300");
  double beta = CLP.as_double('b');
  n = CLP.as_int('n');

  sprintf(title,"An Ising configuration (beta=%6f)",beta);
  Image img (n,n,2,title);

  low = new char[n*n];
  high = new char[n*n];

  /* conditions initiales / au bord */

#define LEFT 0
#define RIGHT 1

  for (i=0;i<n*n;i++) {
    low[i]=0;
    high[i]=1;
  }

  for (i=0;i<(n>>1);i++) { low[i]=LEFT; low[i+(n-1)*n]=LEFT; }
  for (i=0;i<n;i++) { low[i*n]=LEFT; }
  for (i=(n>>1);i<n;i++) { low[i]=RIGHT; low[i+(n-1)*n]=RIGHT; }
  for (i=0;i<n;i++) { low[i*n+n-1]=RIGHT; }

  for (i=0;i<(n>>1);i++) { high[i]=LEFT; high[i+(n-1)*n]=LEFT; }
  for (i=0;i<n;i++) { high[i*n]=LEFT; }
  for (i=(n>>1);i<n;i++) { high[i]=RIGHT; high[i+(n-1)*n]=RIGHT; }
  for (i=0;i<n;i++) { high[i*n+n-1]=RIGHT; }

  img.show();

  dist = paint(img);

  /* Initialisations */

  p[0]=1;
  p[1]=(long)(exp(-2*beta) * (double)prng.max);
  p[2]=(long)(exp(-4*beta) * (double)prng.max);

  /* Simulation */

  for (bla=1;dist>0;bla*=2) {
    prng.rewind (2*bla,(n-2)*(n-2));
    for (i=0;i<n*n;i++) {
      low[i]=0;
      high[i]=1;
    }

    for (i=0;i<n*img.width;++i) img(i)=2;
    
    for (i=0;i<n;i++) { img.putpoint(0,i,3*LEFT,0); img.putpoint(n-1,i,3*RIGHT,0); }
    for (i=0;i<(n>>1);i++) { img.putpoint(i,0,3*LEFT,0); img.putpoint(i,n-1,3*LEFT,0); }
    for (i=(n>>1);i<n;i++) { img.putpoint(i,0,3*RIGHT,0); img.putpoint(i,n-1,3*RIGHT,0); }
    
    for (i=0;i<(n>>1);i++) { low[i]=LEFT; low[i+(n-1)*n]=LEFT; }
    for (i=0;i<n;i++) { low[i*n]=LEFT; }
    for (i=(n>>1);i<n;i++) { low[i]=RIGHT; low[i+(n-1)*n]=RIGHT; }
    for (i=0;i<n;i++) { low[i*n+n-1]=RIGHT; }
    
    for (i=0;i<(n>>1);i++) { high[i]=LEFT; high[i+(n-1)*n]=LEFT; }
    for (i=0;i<n;i++) { high[i*n]=LEFT; }
    for (i=(n>>1);i<n;i++) { high[i]=RIGHT; high[i+(n-1)*n]=RIGHT; }
    for (i=0;i<n;i++) { high[i*n+n-1]=RIGHT; }
    
    for (j=0;j<bla;j++) {
      one_step();
    }
    dist = paint(img);

    fprintf(stderr,"nstep = %5ld, dist = %ld\n", bla, dist);
  }

  std::cout << img;
  return 0;
}
