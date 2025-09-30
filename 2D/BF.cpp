#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

template <> auto vb::to_Color(double v) -> Color { return Indexed(v > 0 ? 1 : 2); }

struct BF : public Bitmap<double> {
  BF(std::string s, size_t n, double e, double b) : Bitmap(s, ucoo(n, n)), tmp(size), ee(size) {
    for (auto z : coo_range(size)) {
      at(z)  = prng.gaussian();
      tmp[z] = at(z);
      ee.put(z, ((z.x - n / 2) * (z.x - n / 2) + (z.y - n / 2) * (z.y - n / 2) < n * n / 8 ? b : 1) * e);
    }
  }

  void heat_step() {
    parallel_do([this](ucoo z) {
      double e = ee.at(z);
      tmp[z]   = at(z) * (1 - 4 * e);
      for (int i = 0; i < 4; ++i) tmp[z] += e * atp((coo)z + dz[i]);
    });
    parallel_do([this](ucoo z) { at(z) = tmp[z]; });
    step();
  }

  Array<double> tmp, ee;
};

auto main(int argc, char **argv) -> int {
  Hub const H("Inhomogeneous Bargman-Fock", argc, argv, "n=600,e=.001,t=5000,b=.5");
  BF        B(H.title, H['n'], H['e'], H['b']);
  B.show();
  for (int t = 0; t < int(H['t']); ++t) { B.heat_step(); }
  B.output_png(H.title);
}