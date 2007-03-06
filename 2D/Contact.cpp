
#include <vb.h>

using namespace vb;

int main(int argc, char *argv[])
{
  double p;
  long i,nstep,x,y,total,n;
  char title[80];

  /* arguments -> p et n */

  if (argc != 3) {
    std::cerr << "Syntaxe : Contant <p> <n>\n";
    exit(1);
  }
  p=atof(argv[1]);
  n=atoi(argv[2]);

  sprintf(title,"Discrete-time contact process on Z^2 (p=%f)",p);

  Image img (n,n,1,title);
  PRNG prng;

  /* conditions initiales */

  for (int x=0;x<n;++x) {
    img.putpoint(x,0,1,0);
    for (int y=1;y<n;++y)
      img.putpoint(x,y,0,0);
  }

  img.onscreen();

  /* Simulation */

  nstep = 3*n;

  for (i=0;i<nstep;i++) {
    total=0;
    for (x=1;x<n-1;x++) {
      for (y=1;y<n-1;y++) {
	if ((x+y+i)%2) {
	  if ((img(x-1,y)||img(x+1,y)||img(x,y-1)||img(x,y+1)) &&
	      (prng.bernoulli(p))) {
	    img.putpoint(x,y,1);
	    total++;
	  } else {
	    img.putpoint(x,y,0);
	  }
	}
      }
    }
  }
  
  std::cout << img;

  return 0;
}
