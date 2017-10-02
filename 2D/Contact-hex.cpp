#include <vb/Image.h>

using namespace vb;

int main(int argc, char *argv[])
{
  long i,nstep,x,y,total;

  /* arguments -> p et n */

  H.init ("Discrete-time contact process on the hexagonal lattice",argc,argv,"p=.5,n=400");
  double p = H['p'];
  int64_t n = H['n'];

  Image img (n,n);

  /* conditions initiales */

  for (int x=0;x<n;++x)
    for (int y=0;y<n/2;++y)
      img.put(coo(x,y),255);

  img.show();

  /* Simulation */

  nstep = 3*n;

  for (i=0;i<nstep;i++) {
    total=0;
    for (x=1;x<n-1;x++) {
      for (y=1;y<n-1;y++) {
        if ((x+y+i)%3 == 0) {
          if ((img.at(coo(x-1,y-1))||img.at(coo(x,y+1))||img.at(coo(x+1,y))) &&
              (prng.bernoulli(p))) {
            img.put(coo(x,y),255);
            total++;
          } else {
            img.put(coo(x,y),0);
          }
        }
      }
    }
    img.update();
    if (total==0) { i=nstep; }
  }

  return 0;
}
