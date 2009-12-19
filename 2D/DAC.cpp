
/*
 * DAC.cpp - Divide-and-Color model on the square lattice
 */

#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace vb;

int main(int argc, char ** argv)
{
  // Parameters from the command line

  CL_Parser CLP (argc,argv,"n=300,p=.3,q=.4");
  int n = CLP('n');
  double p = CLP('p');
  double q = CLP('q');

  // Connectivity (&1 -> to the right, &2 -> downwards)

  PRNG prng;

  char *connect = new char[n*n];
  for (int i=0; i<n*n; ++i)
    connect[i] = prng.bernoulli(p) + 2*prng.bernoulli(p);

  // Stupid algorithm to extract connected components. At the end,
  // cluster[i+n*j] is the label of the component of (i,j), between 0
  // and n*n (not successive or anything, but unique).

  std::cerr << "Computing connected components ...";

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

  std::cerr << " Done." << std::endl;

  // Color each cluster with probability q and output the result :

  char *color = new char[n*n];
  for (int i=0; i<n*n; ++i)
    color[i] = (char) prng.bernoulli(q);

  char title[100];
  sprintf(title,"Divide-and-color model (p=%.3f,q=%.3f)",p,q);
  Image img (n,n,title);

  for (int x=0; x<n; ++x)
    for (int y=0; y<n; ++y)
      img.putpoint(x,y,color[cluster[x+n*y]]);

  return 0;
}
