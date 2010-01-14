#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/Image.h>

using namespace vb;

PRNG prng;

class Snake : public Image {
  public:
    Snake (int n_);
    void step (int dx, int dy);
    void shrink ();
    bool alive () const;
  private:
    std::vector<int> x,y;
};

Snake::Snake (int n_) : Image(2*n_, 2*n_, "Self-avoiding snake"), x(1,n_), y(1,n_) {
  putpoint (n_, n_, 255);
}

void Snake::step (int dx, int dy) {
  int nx=x.back()+dx, ny=y.back()+dy;
  if (at(nx,ny) == Color(255)) return;
  x.push_back(nx); y.push_back(ny);
  putpoint (nx,ny,255);
}

void Snake::shrink () {
  if (x.size() == 1) return;
  putpoint (x.back(), y.back(), 0);
  x.pop_back(); y.pop_back();
}

bool Snake::alive () const {
  int lx=x.back(), ly=y.back();
  return ((lx>0) && (lx<width-1) && (ly>0) && (ly<height-1));
}

int main (int argc, char ** argv) {
  CL_Parser CLP (argc, argv, "n=300,e=.5");
  double e = CLP('e');

  Snake S(CLP('n')); S.show();

  while (S.alive()) {
    if (prng.bernoulli(e)) S.shrink(); else {
      int dx=0, dy=0;
      switch ((int) prng.uniform(4)) {
        case 0: dx=1;  break;
        case 1: dx=-1; break;
        case 2: dy=1;  break;
        case 3: dy=-1; break;
      }
      S.step (dx,dy);
    }
  }
  S.output(); return 0;
}
