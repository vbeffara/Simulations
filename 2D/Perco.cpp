#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace vb;

int main(int argc, char ** argv) {
  CL_Parser CLP (argc, argv, "n=500,p=.5,d,s=0");
  int    n = CLP('n'); n -= (n%4);
  double p = CLP('p');
  bool   d = CLP('d');
  int    s = CLP('s');

  Image img (n,n);

  for (int x=0; x<n; ++x) {
    for (int y=0; y<n; ++y) {
      if      ((x%2 == 0) && (y%2 == 0)) img.put (coo(x,y), 128);
      else if ((x%2 == 0) || (y%2 == 0)) img.put (coo(x,y), prng.bernoulli(p) * 128);
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

      Color c = prng.bernoulli(p) * 128;
      if (img.at(coo(x,y)) == c) continue;
      if (on_it && (c == Color(128))) continue;

      img.fill (coo(n/2,n/2), 128);
      img.put (coo(x,y), c);
      img.fill (coo(n/2,n/2), Color(255,0,0));

      img.step();
    }
  } else img.pause();

  return 0;
}
