
#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace vb;

#define MIRROR_NONE    Color(0)
#define MIRROR_NE      Color(255,0,0)
#define MIRROR_NW      Color(255,255,0)
#define MIRROR_FLIP_NE Color(0,0,255)
#define MIRROR_FLIP_NW Color(0,255,0)

int dx[4] = {1,0,-1,0};
int dy[4] = {0,1,0,-1};

int flip_ne[4] = {1,0,3,2};
int flip_nw[4] = {3,2,1,0};

int main (int argc, char ** argv) {
  CL_Parser CLP (argc, argv, "n=200,p=.5,q=0,f=0,s=0");
  
  int n = CLP('n'); // Size of the box
  double p = CLP('p'); // Initial asymmetry of the mirrors
  double q = CLP('q'); // Density of holes
  double f = CLP('f'); // Density of flippable among mirrors
  int s = CLP('s'); // The initial seed of the prng

  prng.seed(s);

  Image img (n,n, "The mirror model");
  img.show();

  for (int i=0; i<n; ++i) {
    for (int j=0; j<n; ++j) {
      if (prng.bernoulli(1-q)) {
        if (prng.bernoulli(f)) img.putpoint (i,j, prng.bernoulli(p) ? MIRROR_FLIP_NE : MIRROR_FLIP_NW);
        else                   img.putpoint (i,j, prng.bernoulli(p) ? MIRROR_NE : MIRROR_NW);
      } else                   img.putpoint (i,j, MIRROR_NONE);
    }
  }

  int x=n/2, y=n/2, d=0;

  while ((x>0)&&(y>0)&&(x<n-1)&&(y<n-1)) {
    x += dx[d]; y += dy[d];
    if ((img(x,y) == MIRROR_NE) || (img(x,y) == MIRROR_FLIP_NE)) d = flip_ne[d];
    if ((img(x,y) == MIRROR_NW) || (img(x,y) == MIRROR_FLIP_NW)) d = flip_nw[d];
    if (img(x,y) == MIRROR_FLIP_NE) img.putpoint(x,y,MIRROR_FLIP_NW);
    else if (img(x,y) == MIRROR_FLIP_NW) img.putpoint(x,y,MIRROR_FLIP_NE);
  }

  img.pause();
  return 0;
}