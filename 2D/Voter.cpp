#include <vb.h>

using namespace vb;

int main (int argc, char **argv) {
  CL_Parser CLP (argc,argv,"n=500,p=.5,d=1");
  int n = CLP.as_int('n');
  double p = CLP.as_double('p');
  int d = CLP.as_int('d');

  Image img (n,n,d,"Voter model");

  PRNG prng;

  for (int i=0;i<n;++i) {
    for (int j=0;j<n;++j) {
      if (d>1) img.putpoint(i,j,(int)prng.uniform((1<<d)));
      else img.putpoint(i,j,prng.bernoulli(p));
    }
  }

  img.show();

  for (long t=0;;++t) {
    int i = (int)prng.uniform(n);
    int j = (int)prng.uniform(n);
    char newcolor = img(i,j);

    switch ((int)prng.uniform(4)) {
      case 0:
        if (i<n-1) newcolor = img(i+1,j);
        break;
      case 1:
        if (i>0) newcolor = img(i-1,j);
        break;
      case 2:
        if (j<n-1) newcolor = img(i,j+1);
        break;
      case 3:
        if (j>0) newcolor = img(i,j-1);
        break;
    }
    img.putpoint (i,j,newcolor);
  }

  return 0;
}
