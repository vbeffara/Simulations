#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
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
  CLP clp(argc, argv, "Inhomogeneous Bargman-Fock");
  auto n = clp.param("n", size_t(600), "Grid size");
  auto e = clp.param("e", 0.001, "Diffusion rate");
  auto t = clp.param("t", 5000, "Number of steps");
  auto b = clp.param("b", 0.5, "Inner region factor");
  clp.finalize();
  BF B(clp.title, n, e, b);
  B.show();
  for (int i = 0; i < t; ++i) { B.heat_step(); }
  B.output_png(clp.title);
}