#include <vb/Image.h>


using namespace vb;

int main(int argc, char ** argv) {
  H.init ("Percolation", argc, argv, "n=500,p=.5,d,s=0,t=128");
  int    n = H['n']; n -= (n%4);
  double p = H['p'];
  bool   d = H['d'];
  int    s = H['s'];
  int tint = H['t'];

  Image img (n,n);

  for (int x=0; x<n; ++x) {
    for (int y=0; y<n; ++y) {
      if      ((x%2 == 0) && (y%2 == 0)) img.put (coo(x,y), tint);
      else if ((x%2 == 0) || (y%2 == 0)) img.put (coo(x,y), prng.bernoulli(p) * tint);
    }
  }

  img.fill (coo(n/2,n/2), Color(255,0,0));
  img.show();

  if (d) {
    int t=0;
    while (true) {
      if ((s>0) && ((t++%s)==0)) img.snapshot();
      int x = prng.uniform_int(n);
      int y = prng.uniform_int(n);
      if ((x+y)%2 == 0) continue;
      bool on_it = (img.at(coo(x,y)) == Color(255,0,0));

      Color c = prng.bernoulli(p) * tint;
      if (img.at(coo(x,y)) == c) continue;
      if (on_it && (c == Color(tint))) continue;

      img.fill (coo(n/2,n/2), tint);
      img.at (coo(x,y)) = c;
      img.fill (coo(n/2,n/2), Color(255,0,0));

      img.step();
    }
  } else img.pause();

  return 0;
}
