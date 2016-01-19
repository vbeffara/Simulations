
#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace std;
using namespace vb;

int main (int argc, char **argv) {
  H.init ("Last-passage percolation", argc,argv, "n=500,r=.1,l=.01,d=0");
  int n = H['n'];
  double r = H['r'];
  double l = H['l'];
  double d = H['d'];

  Image img (n,n);
  img.show();

  vector <int> field (n);

  for (int x=0; x<n; ++x)
    if (prng.bernoulli(r)) {
      img.put(coo(x,0),255);
      field[x]=1;
    } else {
      field[x]=0;
    }

  img.update();

  for (int y=1; y<n; ++y) {
    for (int x=0; x<n; ++x)
      if  ( (prng.bernoulli(l)) ||
            ((x==y)&&(prng.bernoulli(d))) ) {
        img.put(coo(x,y),255);
        field[x]++;
        int xx=(x+1)%n;
        while (field[xx]==0) {
          img.put(coo(xx,y),255);
          xx = (xx+1)%n;
        }
        field[xx]--;
        img.put(coo(xx,y),255);
      }
    for (int x=0; x<n; ++x)
      if (field[x])
        img.put(coo(x,y),255);

    if (!(y%100)) img.update();
  }

  return 0;
}
