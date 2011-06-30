#include <vb/Image.h>
#include <vb/CL_Parser.h>
#include <vb/ProgressBar.h>

using namespace vb;

const Color C[4] = { Color(255,0,0), Color(0,255,0), Color(0,0,255), Color(255,255,0) };

int main (int argc, char ** argv) {
  CL_Parser CLP (argc, argv, "n=500");
  int n = CLP('n');

  Image img (n,n,"The Rotor-Router model");
  if (n<=500) img.show();

  int x=n/2, y=n/2;

  { ProgressBar PB (n*n*.785398, 2);
    int surf=0;

    while ((x>0) && (x<n-1) && (y>0) && (y<n-1)) {
      Color c = img(x,y);
      if      (c == C[0]) { img.putpoint (x,y,C[1],0); ++x; }
      else if (c == C[1]) { img.putpoint (x,y,C[2],0); ++y; }
      else if (c == C[2]) { img.putpoint (x,y,C[3],0); --x; }
      else if (c == C[3]) { img.putpoint (x,y,C[0],0); --y; }
      else                { img.putpoint (x,y,C[0]); x=n/2; y=n/2; PB.set(++surf); }
    }
  }

  img.output_png();
}
