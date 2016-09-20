#include <vb/Image.h>

using namespace vb;

int main(int argc, char ** argv)
{
  // Parameters from the command line

  H.init ("Divide-and-color model", argc,argv, "n=300,p=.3,q=.4");
  int n = H['n'];
  double p = H['p'];
  double q = H['q'];

  // Connectivity (&1 -> to the right, &2 -> downwards)

  char *connect = new char[n*n];
  for (int i=0; i<n*n; ++i)
    connect[i] = prng.bernoulli(p) + 2*prng.bernoulli(p);

  // Stupid algorithm to extract connected components. At the end,
  // cluster[i+n*j] is the label of the component of (i,j), between 0
  // and n*n (not successive or anything, but unique).

  H.L->info ("Computing connected components ...");

  unsigned long *cluster = new unsigned long [n*n];
  for (int i=0; i<n*n; ++i)
    cluster[i] = i;

  int dirty = 1;
  while (dirty) {
    dirty = 0;
    for (int x=0;x<n;++x) {
      for (int y=0;y<n;++y) {
        if ((x>0) && (connect[(x-1)+n*y] & 1) && (cluster[x+n*y] > cluster[(x-1)+n*y])) {
          dirty = 1;
          cluster[x+n*y] = cluster[(x-1)+n*y];
        }
        if ((x<n-1) && (connect[x+n*y] & 1) && (cluster[x+n*y] > cluster[(x+1)+n*y])) {
          dirty = 1;
          cluster[x+n*y] = cluster[(x+1)+n*y];
        }
        if ((y>0) && (connect[x+n*(y-1)] & 2) && (cluster[x+n*y] > cluster[x+n*(y-1)])) {
          dirty = 1;
          cluster[x+n*y] = cluster[x+n*(y-1)];
        }
        if ((y<n-1) && (connect[x+n*y] & 2) && (cluster[x+n*y] > cluster[x+n*(y+1)])) {
          dirty = 1;
          cluster[x+n*y] = cluster[x+n*(y+1)];
        }
      }
    }
  }

  H.L->info (" ... Done.");

  // Color each cluster with probability q and output the result :

  char *color = new char[n*n];
  for (int i=0; i<n*n; ++i)
    color[i] = (char) prng.bernoulli(q);

  Image img (n,n);

  for (int x=0; x<n; ++x)
    for (int y=0; y<n; ++y)
      img.put(coo(x,y),color[cluster[x+n*y]]);

  delete[] connect; delete[] color; delete[] cluster;

  return 0;
}
