#include <vb/CL_Parser.h>
#include <vb/Image.h>
#include <vb/PRNG.h>

using namespace vb;

class Painter : public Image {
  public:
    Painter (int w, int h, std::string s) : Image (w,h,s) { };
    void paint (int x, int y, Color c);
};

void Painter::paint (int x, int y, Color c) {
  Color prev = at(x,y);
  if (c == prev) return;

  putpoint (x,y,c);
  std::vector<int> xx (1,x), yy (1,y);

  while (xx.size()) {
    int i = xx.back(); xx.pop_back();
    int j = yy.back(); yy.pop_back();

    if ((i<width-1)  && (at(i+1,j)==prev)) { at(i+1,j) = c; xx.push_back(i+1); yy.push_back(j);   }
    if ((i>0)        && (at(i-1,j)==prev)) { at(i-1,j) = c; xx.push_back(i-1); yy.push_back(j);   }
    if ((j<height-1) && (at(i,j+1)==prev)) { at(i,j+1) = c; xx.push_back(i);   yy.push_back(j+1); }
    if ((j>0)        && (at(i,j-1)==prev)) { at(i,j-1) = c; xx.push_back(i);   yy.push_back(j-1); }
  }
}

int main(int argc, char ** argv) {
  CL_Parser CLP (argc, argv, "n=500,p=.5");
  int    n = CLP('n');
  double p = CLP('p');

  Painter img (n, n, fmt("Bond percolation (p=%)")%p);

  for (int x=0; x<n; ++x) {
    for (int y=0; y<n; ++y) {
      if      ((x%2 == 0) && (y%2 == 0)) img.putpoint (x, y, 128);
      else if ((x%2 == 0) || (y%2 == 0)) img.putpoint (x, y, prng.bernoulli(p) * 128);
    }
  }

  img.paint (n/2, n/2, Color(255,0,0));

  img.show();
  img.pause();

  return 0;
}
