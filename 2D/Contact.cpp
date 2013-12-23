#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>
#include <vb/fmt.h>

using namespace vb;

int main(int argc, char *argv[])
{
  long i,nstep,x,y,total;

  /* arguments -> p et n */

  CL_Parser CLP (argc,argv,"p=.4,n=400");
  double p = CLP('p');
  int n = CLP('n');

  Image img (n,n, str(fmt("Discrete-time contact process on Z^2 (p=%f)")%p));

  /* conditions initiales */

  for (int x=0;x<n;++x) {
    img.put(coo(x,0),255);
    for (int y=1;y<n;++y)
      img.put(coo(x,y),0);
  }

  img.show();

  /* Simulation */

  nstep = 3*n;

  for (i=0;i<nstep;i++) {
    total=0;
    for (x=1;x<n-1;x++) {
      for (y=1;y<n-1;y++) {
	if ((x+y+i)%2) {
	  if ((img.at(coo(x-1,y))||img.at(coo(x+1,y))||img.at(coo(x,y-1))||img.at(coo(x,y+1))) &&
	      (prng.bernoulli(p))) {
	    img.put(coo(x,y),255);
	    total++;
	  } else {
	    img.put(coo(x,y),0);
	  }
	}
      }
    }
  }

  return 0;
}
