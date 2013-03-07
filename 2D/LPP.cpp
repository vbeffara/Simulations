
#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace std;
using namespace vb;

int main (int argc, char **argv) {
  CL_Parser CLP (argc,argv, "n=500,r=.1,l=.01,d=0");
  int n = CLP('n');
  double r = CLP('r');
  double l = CLP('l');
  double d = CLP('d');

  Image img (n,n,"Broken line process");
  img.show();

  vector <int> field (n);

  for (int x=0; x<n; ++x)
    if (prng.bernoulli(r)) {
      img.putpoint(x,0,1);
      field[x]=1;
    } else {
      field[x]=0;
    }

  img.update();

  for (int y=1; y<n; ++y) {
    for (int x=0; x<n; ++x)
      if  ( (prng.bernoulli(l)) ||
            ((x==y)&&(prng.bernoulli(d))) ) {
        img.putpoint(x,y,1);
        field[x]++;
        int xx=(x+1)%n;
        while (field[xx]==0) {
          img.putpoint(xx,y,1);
          xx = (xx+1)%n;
        }
        field[xx]--;
        img.putpoint(xx,y,1);
      }
    for (int x=0; x<n; ++x)
      if (field[x])
        img.putpoint(x,y,1);

    if (!(y%100)) img.update();
  }

  return 0;
}
