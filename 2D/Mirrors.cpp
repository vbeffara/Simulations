#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>
#include <vb/dxdy.h>

using namespace vb;
using namespace std;

#define STATE_NONE    0
#define STATE_PRESENT 128
#define STATE_NE      1
#define STATE_FLIP    2
#define STATE_VISITED 4

#define MIRROR_NONE    Color(0)
#define MIRROR_NE      Color(255,0,0)
#define MIRROR_NW      Color(255,255,0)
#define MIRROR_FLIP_NE Color(0,0,255)
#define MIRROR_FLIP_NW Color(0,255,0)
#define MIRROR_NE_VISITED      Color(128,0,0)
#define MIRROR_NW_VISITED      Color(128,128,0)
#define MIRROR_FLIP_NE_VISITED Color(0,0,128)
#define MIRROR_FLIP_NW_VISITED Color(0,128,0)

Color colors[8] = {
  MIRROR_NW, MIRROR_NE, MIRROR_FLIP_NW, MIRROR_FLIP_NE,
  MIRROR_NW_VISITED, MIRROR_NE_VISITED, MIRROR_FLIP_NW_VISITED, MIRROR_FLIP_NE_VISITED,
};

int flip_ne[4] = {1,0,3,2};
int flip_nw[4] = {3,2,1,0};

class Mirror {
public:
  Mirror (unsigned char s_ = 0) : s(s_) {}
  unsigned char s;
  operator unsigned char () { return s; }
  operator Color () {
    if (s==0) return 0;
    if (s==STATE_VISITED) return 128;
    return colors[s%8];
  }
};

class Mirrors : public Bitmap<Mirror> {
public:
  Mirrors (CL_Parser &CLP) : Bitmap<Mirror> (CLP('n'),CLP('n'),"The mirror model"),
                             n(CLP('n')), p(CLP('p')), q(CLP('q')), f(CLP('f')) {}
  int main ();
  int n;
  double p, q, f;
};

int Mirrors::main () {
  show();

  while (true) {
    for (int i=0; i<n; ++i) {
      for (int j=0; j<n; ++j) {
        at(coo(i,j)) = STATE_NONE;
        if (prng.bernoulli(1-q)) {
          at(coo(i,j)).s |= STATE_PRESENT;
          if (prng.bernoulli(p)) at(coo(i,j)).s |= STATE_NE;
          if (prng.bernoulli(f)) at(coo(i,j)).s |= STATE_FLIP;
        }
      }
    }

    int x=n/2, y=n/2, d=0, t=0;

    while ((t<8*n*n)&&(x>0)&&(y>0)&&(x<n-1)&&(y<n-1)) {
      ++t;
      at(coo(x,y)).s |= STATE_VISITED;

      x += dx[d]; y += dy[d];

      if (at(coo(x,y)) & STATE_PRESENT) {
        if (at(coo(x,y)).s & STATE_NE) d = flip_ne[d]; else d = flip_nw[d];
        if (at(coo(x,y)).s & STATE_FLIP) at(coo(x,y)).s ^= STATE_NE;
      }
    }

    pause();
  }
}

int main (int argc, char ** argv) {
  CL_Parser CLP (argc, argv, "n=200,p=.5,q=0,f=0,s=0");
  prng.seed((int)CLP('s'));
  Mirrors(CLP).main();
}
