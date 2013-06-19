#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>
#include <vb/common.h> // fmt

using namespace vb;

int main(int argc, char *argv[])
{
  long i,x,y,xy,f1,f2,nstep;
  unsigned long p[5];

  /* arguments -> beta et n
   * beta_critical = log(1+sqrt(2)) = 0.88137359...
   */

  CL_Parser CLP (argc,argv,"b=.88137359,n=300");
  double beta = CLP('b');
  long n = CLP('n');

  Image img(n,n, str(fmt("An Ising configuration (beta=%f)")%beta));

  /* conditions initiales / au bord */

#define LEFT 0
#define RIGHT 255

  if (beta > log(1+sqrt(2.0))) { /* Basse temp. */
    for (x=1;x<n-1;x++) {
      for (y=1;y<n-1;y++) {
        img.putpoint (x,y, (x<(n>>1)?LEFT:RIGHT));
      }
    }
  } else { /* Haute temp. */
    for (x=1;x<n-1;x++) {
      for (y=1;y<n-1;y++) {
        img.putpoint (x,y, (prng.bernoulli(.5)?LEFT:RIGHT));
      }
    }
  }

  for (i=0;i<(n>>1);i++) { img.putpoint (i,0,LEFT);  img.putpoint (i,n-1,LEFT); }
  for (i=0;i<n;i++)      { img.putpoint (0,i,LEFT); }
  for (i=(n>>1);i<n;i++) { img.putpoint (i,0,RIGHT); img.putpoint (i,n-1,RIGHT); }
  for (i=0;i<n;i++)      { img.putpoint (n-1,i,RIGHT); }

  img.show();

  /* Initialisations */

  p[0]=0;
  p[1]=(long)(exp(-beta) * prng.max());
  p[2]=(long)(exp(-2*beta) * prng.max());
  p[3]=(long)(exp(-3*beta) * prng.max());
  p[4]=(long)(exp(-4*beta) * prng.max());

  /* Simulation */

  nstep = (int) ( 10 + n * 0.01 / fabs(beta-log(1.0+sqrt(2.0))) );

  for (i=0;i<nstep;i++) { /* Ouais bof */
    for (x=1;x<n-1;x++) {
      for (y=1;y<n-1;y++) {
        f1 = 0; f2 = 0; xy = x + n*y;

        if (img.at(coo(xy,0)) == img.at(coo(xy-1,0))) { f2++; } else { f1++; }
        if (img.at(coo(xy,0)) == img.at(coo(xy-n,0))) { f2++; } else { f1++; }
        if (img.at(coo(xy,0)) == img.at(coo(xy+1,0))) { f2++; } else { f1++; }
        if (img.at(coo(xy,0)) == img.at(coo(xy+n,0))) { f2++; } else { f1++; }

        if ( (f2<=f1) || (prng() < p[f2-f1]) ) {
          img.putpoint(x,y,(255-img.at(coo(xy,0))));
        }
      }
    }
  }

  img.output();

  return 0;
}
