
#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace vb;

int main (int argc, char **argv) {
  CL_Parser CLP (argc,argv,"n=500,p=.5,d=1");
  int n = CLP('n');
  double p = CLP('p');
  int d = CLP('d');

  Image img (n,n,"Voter model");

  for (int i=0;i<n;++i) {
    for (int j=0;j<n;++j) {
      if (d>1) img.putpoint(i,j,(int)prng.uniform_int((1<<d)));
      else img.putpoint(i,j,prng.bernoulli(p));
    }
  }

  img.show();

  for (long t=0;;++t) {
    int i = (int)prng.uniform_int(n);
    int j = (int)prng.uniform_int(n);
    char newcolor = img.at(coo(i,j));

    switch ((int)prng.uniform_int(4)) {
      case 0:
        if (i<n-1) newcolor = img.at(coo(i+1,j));
        break;
      case 1:
        if (i>0) newcolor = img.at(coo(i-1,j));
        break;
      case 2:
        if (j<n-1) newcolor = img.at(coo(i,j+1));
        break;
      case 3:
        if (j>0) newcolor = img.at(coo(i,j-1));
        break;
    }
    img.putpoint (i,j,newcolor);
  }

  return 0;
}
