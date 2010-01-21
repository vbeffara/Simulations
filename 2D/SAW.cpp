#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/Path.h>
#include <vb/ProgressBar.h>

using namespace std;
using namespace vb;

int main(int argc, char ** argv)
{
  CL_Parser CLP (argc,argv,"n=500,t=5000");
  int n = CLP('n');
  int t = CLP('t');

  char title[100]; sprintf (title,"A self-avoiding walk of length %d",n);

  Path P (n,title,true);
  ProgressBar PB (t);

  for (int i=0;i<t;) {
    PB.update(i);
    int j = prng.rand()%n;
    int k = 1+(prng.rand()%3);
    int l = P[j]; P[j]=(P[j]+k)%4;
    if (P.self_avoiding()) i++; else P[j]=l;
  }

  PB.die();
  P.output();
  return 0;
}
