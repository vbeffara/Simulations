#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/CoarseImage.h>
#include <vb/Path.h>

using namespace vb;

class Snake : public CoarseImage {
  public:
    Snake (int n_);
    void step (int dx, int dy);
    void shrink ();
    bool alive () const;
    void output () const;
  private:
    std::vector<int> x,y;
};

Snake::Snake (int n_) : CoarseImage (2*n_, 2*n_, pow(n_,.333), "Self-avoiding snake"), x(1,n_), y(1,n_) {
  putpoint (n_, n_, 1);
}

void Snake::step (int dx, int dy) {
  int nx=x.back()+dx, ny=y.back()+dy;
  if (at(nx,ny) == 1) return;
  x.push_back(nx); y.push_back(ny);
  putpoint (nx,ny,1);
}

void Snake::shrink () {
  if (x.size() == 1) return;
  putpoint (x.back(), y.back(), 0);
  x.pop_back(); y.pop_back();
}

bool Snake::alive () const {
  int lx=x.back(), ly=y.back();
  return ((lx>0) && (lx<true_width-1) && (ly>0) && (ly<true_height-1));
}

void Snake::output () const {
  Path P (x.size()-1, title);
  for (unsigned i=0; i<x.size()-1; ++i) {
    if (x[i+1]>x[i]) P[i]=0;
    if (x[i+1]<x[i]) P[i]=2;
    if (y[i+1]>y[i]) P[i]=1;
    if (y[i+1]<y[i]) P[i]=3;
  }
  P.output();
}

int main (int argc, char ** argv) {
  CL_Parser CLP (argc, argv, "n=1000,a=.38");
  double a = CLP('a');
  double e = 1.0 / (1.0 + 4*a);

  Snake S(CLP('n')); S.show();

  while (S.alive()) {
    if (prng.bernoulli(e)) S.shrink(); else {
      int dx=0, dy=0;
      switch ((int) prng.uniform_int(4)) {
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
