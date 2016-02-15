#include <vb/Image.h>

using namespace vb;

class Glauber : public Image {
  public:
    Glauber (int n_) : Image(n_,n_), n(n_) { };
    void fill (double p);
    void step (int i=-1, int j=-1);
  private:
    int n;
};

void Glauber::fill (double p) {
  for (int i=0; i<n; ++i)
    for (int j=0; j<n; ++j)
      if (prng.bernoulli(p)) put(coo(i,j),255);
      else put(coo(i,j),0);
}

void Glauber::step (int i, int j) {
  if (i==-1) i=prng.uniform_int(n);
  if (j==-1) j=prng.uniform_int(n);

  int c=0;
  if (at(coo((i+1)%n,   j)) == Color(255)) ++c;
  if (at(coo((i+n-1)%n, j)) == Color(255)) ++c;
  if (at(coo(i,   (j+1)%n)) == Color(255)) ++c;
  if (at(coo(i, (j+n-1)%n)) == Color(255)) ++c;

  if ((c>2) || ((c==2) && prng.bernoulli(.5))) put(coo(i,j),255);
  else put(coo(i,j),0);
}

int main (int argc, char ** argv) {
  H.init ("Glauber dynamics at zero temperature", argc,argv, "n=500,p=.51");
  int    n = H['n'];
  double p = H['p'];

  Glauber G(n);
  G.fill(p);
  G.show();

  while (true) G.step();
  return 0;
}
