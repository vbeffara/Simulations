
#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

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

int dx[4] = {1,0,-1,0};
int dy[4] = {0,1,0,-1};

int flip_ne[4] = {1,0,3,2};
int flip_nw[4] = {3,2,1,0};

class Mirror {
public:
  Mirror (unsigned char s_ = 0) : s(s_) {}
  unsigned char s;
  operator unsigned char () { return s; }
  operator Color () { return colors[s]; }
};

class Mirrors : public Image {
public:
  Mirrors (CL_Parser &CLP) : Image(CLP('n'),CLP('n'),"The mirror model"), n(CLP('n')), p(CLP('p')), q(CLP('q')), f(CLP('f')) {}
  int main ();
  int n;
  double p, q, f;
};

int Mirrors::main () {
  vector<Mirror> state (n*n);

  show();

  while (true) {
    for (int i=0; i<n; ++i) {
      for (int j=0; j<n; ++j) {
        state[i+n*j] = STATE_NONE;
        putpoint (i,j,0);
        if (prng.bernoulli(1-q)) {
          state[i+n*j].s |= STATE_PRESENT;
          if (prng.bernoulli(p)) state[i+n*j].s |= STATE_NE;
          if (prng.bernoulli(f)) state[i+n*j].s |= STATE_FLIP;
          putpoint (i,j,colors[state[i+n*j]%8]);
        }
      }
    }

    int x=n/2, y=n/2, d=0, t=0;

    while ((t<8*n*n)&&(x>0)&&(y>0)&&(x<n-1)&&(y<n-1)) {
      ++t;
      state[x+n*y].s |= STATE_VISITED;
      if (state[x+n*y].s & STATE_PRESENT) putpoint (x,y,colors[state[x+n*y]%8]);

      x += dx[d]; y += dy[d];

      if (state[x+n*y] & STATE_PRESENT) {
        if (state[x+n*y].s & STATE_NE) d = flip_ne[d]; else d = flip_nw[d];
        if (state[x+n*y].s & STATE_FLIP) state[x+n*y].s ^= STATE_NE;
        putpoint (x,y, colors[state[x+n*y]%8]);
      }
    }

    pause();
  }

  return 0;
}

int main (int argc, char ** argv) {
  CL_Parser CLP (argc, argv, "n=200,p=.5,q=0,f=0,s=0");
  prng.seed((int)CLP('s'));
  return Mirrors(CLP).main();
}
