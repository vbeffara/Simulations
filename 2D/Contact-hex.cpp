
#include <vb.h>

using namespace vb;

int main(int argc, char *argv[])
{
  long i,nstep,x,y,total;
  char title[80];

  /* arguments -> p et n */

  CL_Parser CLP (argc,argv,"p=.5,n=400");
  double p = CLP('p');
  int n = CLP('n');

  sprintf(title,"Discrete-time contact process on the tri/hex lattice (p=%f)",p);

  Image img (n,n,1,title);
  PRNG prng;

  /* conditions initiales */

  for (int x=0;x<n;++x)
    for (int y=0;y<n/2;++y)
      img.putpoint(x,y,1,0);

  img.show();

  /* Simulation */

  nstep = 3*n;

  for (i=0;i<nstep;i++) {
    total=0;
    for (x=1;x<n-1;x++) {
      for (y=1;y<n-1;y++) {
        if ((x+y+i)%3 == 0) {
          if ((img(x-1,y-1)||img(x,y+1)||img(x+1,y)) &&
              (prng.bernoulli(p))) {
            img.putpoint(x,y,1);
            total++;
          } else {
            img.putpoint(x,y,0);
          }
        }
      }
    }
    img.update();
    if (total==0) { i=nstep; }
  }

  std::cout << img;

  return 0;
}
