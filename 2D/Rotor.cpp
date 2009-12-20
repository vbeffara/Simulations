#include <vb/Image.h>

using namespace vb;

const Color C[4] = { Color(255,0,0), Color(0,255,0), Color(0,0,255), Color(255,255,0) };

int main () {
  Image img (600,600,"Rotor-Router");
  img.show();

  int x=300, y=300;

  while ((x>0) && (x<600-1) && (y>0) && (y<600-1)) {
    Color c = img(x,y);
    if (c == C[0]) {
      img.putpoint (x,y,C[1],0);
      ++x;
    } else if (c == C[1]) {
      img.putpoint (x,y,C[2],0);
      ++y;
    } else if (c == C[2]) {
      img.putpoint (x,y,C[3],0);
      --x;
    } else if (c == C[3]) {
      img.putpoint (x,y,C[0],0);
      --y;
    } else {
      img.putpoint (x,y,C[0]);
      x=300; y=300;
    }
  }
}
