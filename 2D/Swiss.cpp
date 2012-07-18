
#include <vb/Bitmap.h>
#include <vb/CL_Parser.h>
#include <vb/PRNG.h>

using namespace vb;
using namespace std;

#define EAST  0
#define NORTH 1
#define WEST  2
#define SOUTH 3

#define C_EAST  Color(0,255,0)
#define C_NORTH Color(255,0,0)
#define C_WEST  Color(0,0,255)
#define C_SOUTH Color(255,255,0)

Color colors[4] = { C_EAST, C_NORTH, C_WEST, C_SOUTH }; //

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
  World (CL_Parser &CLP) : Bitmap<Site> (CLP('n'),CLP('n'),"The Swiss journalist model") {
    int mid = (w()+h())/2;
    for (int x=0; x<w(); ++x) {
      for (int y=0; y<h(); ++y) {
        if (y>x) {
          if (x+y<mid) at(x,y) = EAST;
          else at(x,y) = SOUTH;
        } else {
          if (x+y<mid) at(x,y) = NORTH;
          else at(x,y) = WEST;
        }
        if (prng.bernoulli(0)) at(x,y) = prng.uniform_int(4);
      }
    }

    for (int x=100; x<w()-100; ++x)
      for (int y=100; y<h()-100; ++y)
        at(x,y) = prng.uniform_int(4);
  }
};

int main (int argc, char ** argv) {
  CL_Parser CLP (argc,argv,"n=1000");
  World w(CLP);
  w.show();

  int x=w.w()/2, y=w.h()/2;

  while (true) {
    int nb[4] = { 0,0,0,0 };
    nb[w.at((x+1)%(w.w()),y)] += 1;
    nb[w.at((x+w.w()-1)%(w.w()),y)] += 1;
    nb[w.at(x,(y+1)%(w.h()))] += 1;
    nb[w.at(x,(y+w.h()-1)%(w.h()))] += 1;
    int max = 0;
    for (int i=0; i<4; ++i) if (nb[i]>max) max=nb[i];

    int d = prng.uniform_int(4);
    if (prng.bernoulli(.3))
      while (nb[d]<max) d = prng.uniform_int(4);

    w.at(x,y) = d; x = (x+dx[d]+w.w())%(w.w()); y = (y+dy[d]+w.h())%(w.h());
    w.step();
  }
}
