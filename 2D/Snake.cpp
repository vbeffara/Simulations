#include <vb/CL_Parser.h>
#include <vb/PRNG.h>
#include <vb/CoarseImage.h>
#include <vb/Path.h>

using namespace vb;

class Snake : public CoarseImage {
  public:
    Snake (int n);
    void step (coo dz);
    void shrink ();
    bool alive () const;
    void output () const;
  private:
    std::vector<coo> z;
};

Snake::Snake (int n) : CoarseImage (2*n, 2*n, "Self-avoiding snake", pow(n,.333)), z(1,coo(n,n)) {
  put (coo(n,n), 1);
}

void Snake::step (coo dz) {
  coo nz = z.back()+dz;
  if (at(nz) == 1) return;
  z.push_back(nz);
  put (nz,1);
}

void Snake::shrink () {
  if (z.size() == 1) return;
  put (z.back(), 0);
  z.pop_back();
}

bool Snake::alive () const {
  coo lz=z.back();
  int lx=lz.x, ly=lz.y;
  return ((lx>0) && (lx<true_width-1) && (ly>0) && (ly<true_height-1));
}

void Snake::output () const {
  OldPath P (z.size()-1, title);
  for (unsigned i=0; i<z.size()-1; ++i) {
    coo dz = z[i+1]-z[i];
    int dx = dz.x, dy = dz.y;
    if (dx>0) P[i]=0;
    if (dx<0) P[i]=2;
    if (dy>0) P[i]=1;
    if (dy<0) P[i]=3;
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
      S.step (dz[prng.uniform_int(4)]);
    }
  }
  S.output(); return 0;
}
