#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

struct val {
  double v;
         operator Color() const { return vb::Indexed(v > 0 ? 1 : 2); }
};

class BF : public Bitmap<val> {
  public:
  BF(const Hub &H) : Bitmap(H.title, ucoo(H['n'], H['n'])), tmp(size), e(H['e']) {
    for (auto z : vb::coo_range(size)) { at(z).v = prng.gaussian(); }
  }

  void heat_step() {
    for (auto z : vb::coo_range(size)) {
      double ee = (z.y > size.y / 2) ? 2 * e : e;
      tmp[z]    = at(z).v * (1 - 4 * ee);
      for (int i = 0; i < 4; ++i) tmp[z] += ee * atp(coo(z.x, z.y) + dz[i]).v;
    }
    for (auto z : vb::coo_range(size)) { at(z).v = tmp[z]; }
  }

  vb::Array<double> tmp;
  double            e;
};

auto main(int argc, char **argv) -> int {
  Hub const H("Inhomogeneous Bargman-Fock", argc, argv, "n=600,e=.1,t=10");
  int       T = H['t'];
  BF        B(H);
  B.show();
  for (int t = 0; t < T; ++t) {
    B.heat_step();
    B.step();
  }
  B.output_png(H.title);
}