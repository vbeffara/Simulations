
#include <vb/Bitmap.h>
#include <vb/CL_Parser.h>
#include <vb/PRNG.h>

using namespace vb;
using namespace std;

#define ZZ 0
#define V1 1
#define V2 2
#define H1 3
#define H2 4

#define C_ZZ Color(0,0,0)
#define C_V1 Color(0,255,0)
#define C_V2 Color(0,0,255)
#define C_H1 Color(255,0,0)
#define C_H2 Color(255,255,0)

Color colors[5] = { C_ZZ, C_V1, C_V2, C_H1, C_H2 };

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
  World (CL_Parser &CLP) : Bitmap<Site> (CLP('n'),CLP('n'),"Domino tilings in the plane") {
    if (string(CLP('o')) == "aztec") {
      for (int x=0; x<w()/2; ++x)
        for (int y=h()/2-1-x; y<h()/2+1+x; ++y)
          at(coo(x,y))=V1;
      for (int x=w()/2; x<w(); ++x)
        for (int y=x-w()/2; y<h()+w()/2-x; ++y)
          at(coo(x,y))=V2;
    } else if (string(CLP('o')) == "heart") {
      int sh = h()/8 - ((h()/8)%2);
      for (int y=0; y<h()/4; ++y) {
        for (int x=-y+w()/4-1; x<w()/4+1+y; ++x) at(coo(x,y+sh))=H1;
        for (int x=-y+w()/4+w()/2-1; x<w()/4+w()/2+1+y; ++x) at(coo(x,y+sh))=H1;
      }
      for (int y=h()/4; y<h()/4+h()/2; ++y)
        for (int x=y-h()/4; x<w()+h()/4-y; ++x) at(coo(x,y+sh)) = H2;
    } else {
      for (int x=0; x<w(); ++x) {
        for (int y=0; y<h(); ++y) {
          if (y%2) at(coo(x,y))=H1; else at(coo(x,y))=H2;
        }
      }
    }
  }
};

int main (int argc, char ** argv) {
  CL_Parser CLP (argc,argv,"n=600,o=aztec|heart");
  World w(CLP);
  w.show();
  w.pause();

  w.snapshot_setup("arctic",1.0);

  while (true) {
    int x = prng.uniform_int(w.w()-1);
    int y = prng.uniform_int(w.h()-1);
    if ((x+y)%2) {
      if ( (w.at(coo(x,y))==H1) && (w.at(coo(x+1,y))==H1) && (w.at(coo(x,y+1))==H2) && (w.at(coo(x+1,y+1))==H2) ) {
        w.at(coo(x,y))=V1; w.at(coo(x,y+1))=V1; w.at(coo(x+1,y))=V2; w.at(coo(x+1,y+1))=V2; w.step();
      } else if ( (w.at(coo(x,y))==V1) && (w.at(coo(x,y+1))==V1) && (w.at(coo(x+1,y))==V2) && (w.at(coo(x+1,y+1))==V2) ) {
        w.at(coo(x,y))=H1; w.at(coo(x+1,y))=H1; w.at(coo(x,y+1))=H2; w.at(coo(x+1,y+1))=H2; w.step();
      }
    } else {
      if ( (w.at(coo(x,y))==H2) && (w.at(coo(x+1,y))==H2) && (w.at(coo(x,y+1))==H1) && (w.at(coo(x+1,y+1))==H1) ) {
        w.at(coo(x,y))=V2; w.at(coo(x,y+1))=V2; w.at(coo(x+1,y))=V1; w.at(coo(x+1,y+1))=V1; w.step();
      } else if ( (w.at(coo(x,y))==V2) && (w.at(coo(x,y+1))==V2) && (w.at(coo(x+1,y))==V1) && (w.at(coo(x+1,y+1))==V1) ) {
        w.at(coo(x,y))=H2; w.at(coo(x+1,y))=H2; w.at(coo(x,y+1))=H1; w.at(coo(x+1,y+1))=H1; w.step();
      }
    }
  }
}
