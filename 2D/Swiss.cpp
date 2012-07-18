
#include <vb/Bitmap.h>
#include <vb/PRNG.h>

using namespace vb;
using namespace std;

#define N 500

#define EAST  0
#define NORTH 1
#define WEST  2
#define SOUTH 3

#define C_EAST  Color(0,255,0)
#define C_NORTH Color(255,0,0)
#define C_WEST  Color(0,0,255)
#define C_SOUTH Color(255,255,0)

Color colors[4] = {
  C_EAST, C_NORTH, C_WEST, C_SOUTH
}; //

int dx[4] = { 1,0,-1,0 };
int dy[4] = { 0,1,0,-1 };

class Site {
public:
  unsigned char s;
  Site (unsigned char _s) : s(_s) { };
  void operator= (unsigned char _s) { s=_s; }
  operator int () { return s; }
  operator Color () { return colors[s]; }
};

class World : public Bitmap<Site> {
public:
  World () : Bitmap<Site> (N,N,"The Swiss journalist model") {}
};

int main (int argc, char ** argv) {
  World w;
  for (int x=0; x<N; ++x) {
    for (int y=0; y<N; ++y) {
      if (y>x) {
        if (x+y<N) w.at(x,y) = EAST;
        else w.at(x,y) = SOUTH;
      } else {
        if (x+y<N) w.at(x,y) = NORTH;
        else w.at(x,y) = WEST;
      }
      if (prng.bernoulli(.9)) w.at(x,y) = prng.uniform_int(4);
    }
  }

  w.show();
  //  w.pause();

  int x=N/2, y=N/2;

  while ((x>0)&&(x<N-1)&&(y>0)&&(y<N-1)) {
    int state = w.at(x,y);
    int d = prng.uniform_int(4);
    if (prng.bernoulli(.1)) {
      w.at(x,y) = d;
    } else {
      int nb[4] = { 0,0,0,0 };
      nb[w.at(x+1,y)] += 1;
      nb[w.at(x-1,y)] += 1;
      nb[w.at(x,y+1)] += 1;
      nb[w.at(x,y-1)] += 1;
      int max = 0;
      for (int i=0; i<4; ++i) if (nb[i]>max) max=nb[i];
      while (nb[d]<max) d = prng.uniform_int(4);
      w.at(x,y) = d;
    }

    //    state = w.at(x,y);
    x += dx[state]; y += dy[state];
    w.step();
    //    cerr << x << " " << y << endl;
  }
}
