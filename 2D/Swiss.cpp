#include <vb/Bitmap.h>
#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/fmt.h>
#include <vb/dxdy.h>

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
  int c;
  double p,q;

  World (CL_Parser &CLP) : Bitmap<Site> (CLP('n'),CLP('n'),"The Swiss journalist model"),
                           c(CLP('c')), p(CLP('p')), q(CLP('q')) {
    int mid = (w()+h())/2;
    for (int x=0; x<w(); ++x) {
      for (int y=0; y<h(); ++y) {
        if (y>x) { if (x+y<mid) at(coo(x,y)) = EAST;  else at(coo(x,y)) = SOUTH; }
        else     { if (x+y<mid) at(coo(x,y)) = NORTH; else at(coo(x,y)) = WEST;  }
        if (prng.bernoulli(p)) at(coo(x,y)) = prng.uniform_int(4);
      }
    }

    for (int x=c; x<w()-c; ++x)
      for (int y=c; y<h()-c; ++y)
        at(coo(x,y)) = prng.uniform_int(4);
  }
};

int main (int argc, char ** argv) {
  CL_Parser CLP (argc,argv,"n=600,c=0,p=.8,q=.35");
  World w(CLP);
  w.show();
  w.pause();

  int x=w.w()/2, y=w.h()/2;

  while (true) {
    int nb[4] = { 0,0,0,0 };
    nb[w.at(coo((x+1)%(w.w()),y))] += 1;
    nb[w.at(coo((x+w.w()-1)%(w.w()),y))] += 1;
    nb[w.at(coo(x,(y+1)%(w.h())))] += 1;
    nb[w.at(coo(x,(y+w.h()-1)%(w.h())))] += 1;
    int max = 0;
    for (int i=0; i<4; ++i) if (nb[i]>max) max=nb[i];

    int d = prng.uniform_int(4);
    if (prng.bernoulli(1-w.q))
      while (nb[d]<max) d = prng.uniform_int(4);

    w.at(coo(x,y)) = d; x = (x+dx[d]+w.w())%(w.w()); y = (y+dy[d]+w.h())%(w.h());
    w.step();
  }
}
