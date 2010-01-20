#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/Path.h>
#include <vb/ProgressBar.h>

using namespace std;
using namespace vb;

int main(int argc, char ** argv)
{
  int n;                   /*  Nombre de pas */
  int t;                   /*  Durée de la simulation */
  char title[100];

  int i,j,k,done;
  char l=0;

  CL_Parser CLP (argc,argv,"n=500,t=5000");
  n = CLP('n');
  t = CLP('t');

  sprintf (title,"A self-avoiding walk of length %d",n);
  Path P (n,title);

  done = 0;

  ProgressBar PB (t);

  for (i=0;i<t;) {
    PB.update(i);

    j = prng.rand()%n;         /*  Position du pivot */
    k = 1+(prng.rand()%3);     /*  Rotation = 1, 2 ou 3 mod 4 */
    l = P[j]; P[j]=(P[j]+k)%4;
    
    if (P.self_avoiding()) i++;
    else P[j]=l;
  }

  PB.die();

  for (i=0;i<n;i++) {
    l=(l+P[i])%4;
    P[i]=l;
  }

  P.printout ();

  return 0;
}
