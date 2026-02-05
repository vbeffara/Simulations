#include <spdlog/spdlog.h>
#include <vb/Coloring.h>
#include <vb/Constellation0.h>
#include <vb/math/Permutation.h>
#include <vb/util/CLP.h>

using namespace vb;
using namespace std;

auto triangulations(unsigned n) -> Stream2<Hypermap> {
  Cycles phic;
  for (unsigned i = 0; i < n / 3; ++i) phic.emplace_back(std::vector<size_t>{i, i + n / 3, i + 2 * n / 3});
  Permutation           phi(phic);
  unsigned              np = n / 6;
  vector<size_t>        a(n / 2 - np, 2);
  std::vector<Hypermap> hs;

  for (auto alph : permutations(a)) {
    Permutation alpha(n);
    for (unsigned i = 0; i + 2 * np < n; ++i) alpha[i] = alph[i];
    for (unsigned i = 0; i < np; ++i) {
      alpha[n - 2 * np + 2 * i]     = n - 2 * np + 2 * i + 1;
      alpha[n - 2 * np + 2 * i + 1] = n - 2 * np + 2 * i;
    }
    if (!connected(phi, alpha)) continue;
    Permutation sigma = (alpha * phi).inverse();
    Hypermap    h(sigma, alpha, phi);
    h.normalize();
    bool done = false;
    for (auto &hh : hs)
      if (h == hh) {
        done = true;
        break;
      }
    if (!done) {
      hs.push_back(h);
      co_yield (h);
    }
  }
}

auto main(int argc, char **argv) -> int {
  CLP clp(argc, argv, "Spheroidal enumeration");
  auto s = clp.param("s", 3, "Number of vertices");
  auto d = clp.param("d", 2, "Minimum degree");
  auto v = clp.flag("v", "Visualize");
  auto o = clp.flag("o", "Output image");
  auto b = clp.flag("b", "Print Belyi map");
  auto q = clp.flag("q", "Compute high precision");
  clp.finalize();

  unsigned g = 0, a = 6 * (unsigned(s) - 2);

  int nb = 0;
  for (const Hypermap &M : triangulations(a)) {
    if (M.genus() != g) continue;
    bool good = true;
    for (auto &c : M.sigma.cycles())
      if (c.size() < unsigned(d)) good = false;
    if (!good) continue;

    spdlog::info("{}: {}", ++nb, M);

    if (v || o || b || q) {
      auto title = fmt::format("Spheroidal enumeration (s={}, d={}, i={})", s, d, nb);
      Constellation0<double> C{M};
      C.belyi();
      if (b) spdlog::info("{}", C);
      if (v || o) {
        auto     bd = C.bounds();
        Coloring CC(title, bd.first, bd.second, 800, [&](cpx z) { return Indexed((imag(C(z)) > 0) ? 1 : 2); });
        CC.scale(1.5);
        CC.show();
        if (o)
          CC.output(title);
        else {
          while (CC.visible()) {
            CC.update();
            Fl::wait();
          }
        };
        CC.hide();
      }
      if (q) {
        Constellation0<real_t> Cq(C);
        Cq.findn();
        Cq.belyi();
        spdlog::info("{}", Cq);
      }
    }
  }
}
