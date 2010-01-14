
#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/Image.h>

using namespace std;
using namespace vb;

PRNG prng;

class Snake : public Image {
  public:
    Snake (int n_);
    void step (int x, int y);
    void shrink ();
    bool alive ();

    int x,y;
  private:
    int n,l;
    vector<int> path_x;
    vector<int> path_y;
};

Snake::Snake (int n_) : Image(2*n_, 2*n_, "Self-avoiding snake"), x(n_), y(n_), n(n_), l(0), path_x(1,n_), path_y(1,n_) {
  putpoint (x,y,255);
}

void Snake::step (int nx, int ny) {
  if (at(nx,ny) == Color(255)) return;
  x=nx; y=ny;
  putpoint (x,y,255);
  path_x.push_back(x);
  path_y.push_back(y);
  ++l;
}

void Snake::shrink () {
  if (l==0) return;

  putpoint (x,y,0);
  path_x.pop_back(); path_y.pop_back();
  x=path_x.back(); y=path_y.back();
  --l;
}

bool Snake::alive () {
  return ( (x>0) && (x<2*n-1) && (y>0) && (y<2*n-1) );
}

int main (int argc, char ** argv) {
  CL_Parser CLP (argc, argv, "n=300,e=.5");
  int    n = CLP('n');
  double e = CLP('e');

  Snake S(n);
  S.show();

  while (S.alive()) {
    if (prng.bernoulli(e)) S.shrink();
    else {
      int dx=0, dy=0;
      switch ((int) prng.uniform(4)) {
        case 0: dx=1;  break;
        case 1: dx=-1; break;
        case 2: dy=1;  break;
        case 3: dy=-1; break;
      }
      S.step (S.x+dx, S.y+dy);
    }
  }
  return 0;
}
