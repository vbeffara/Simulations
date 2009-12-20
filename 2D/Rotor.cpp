#include <vb/Image.h>
#include <vb/CL_Parser.h>

using namespace vb;

const Color C[4] = { Color(255,0,0), Color(0,255,0), Color(0,0,255), Color(255,255,0) };

int main (int argc, char ** argv) {
  CL_Parser CLP (argc, argv, "n=600");
  int n = CLP('n');

  Image img (n,n,"The Rotor-Router model");
  img.show();

  int x=n/2, y=n/2;

  while ((x>0) && (x<n-1) && (y>0) && (y<n-1)) {
    Color c = img(x,y);
    if      (c == C[0]) { img.putpoint (x,y,C[1],0); ++x; }
    else if (c == C[1]) { img.putpoint (x,y,C[2],0); ++y; }
    else if (c == C[2]) { img.putpoint (x,y,C[3],0); --x; }
    else if (c == C[3]) { img.putpoint (x,y,C[0],0); --y; }
    else                { img.putpoint (x,y,C[0]); x=n/2; y=n/2; }
  }

  img.output_png();
}
