#include <iostream>
#include <vector>
#include <vb/PRNG.h>
#include <vb/CL_Parser.h>
#include <vb/Image.h>

using namespace std;
using namespace vb;

PRNG prng;

void fill_dyadic (vector<double> &f, int n) {
  int nn = 1<<n;
  for (int l=0; l<n; ++l) {
    int ll = 1<<l;
    for (int i=0; i<nn/ll; ++i)
      for (int j=0; j<nn/ll; ++j) {
        double g = prng.gaussian();
        for (int x=i*ll; x<(i+1)*ll; ++x)
          for (int y=j*ll; y<(j+1)*ll; ++y)
            f[x+nn*y] += g;
      }
  }
}

int main (int argc, char **argv) {
  CL_Parser CLP (argc, argv, "n=8");
  int n = CLP('n');
  int nn = 1<<n;

  vector<double> field;
  for (int i=0; i<nn*nn; ++i) field.push_back(0.0);
  fill_dyadic (field,n);

  Image img (nn,nn,1,"Sign of a dyadic GFF");
  for (int i=0; i<nn; ++i)
    for (int j=0; j<nn; ++j)
      img.putpoint (i,j, (field[i+nn*j]>0?1:0));

  cout<<img;
}
