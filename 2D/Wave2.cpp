#include <vb/Coloring.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using vb::cpx;

class Wave2 : public vb::Coloring {
  std::vector<std::vector<double>> coef;
  size_t                           m;

  auto c(cpx z) -> vb::Color {
    double o = z.real() - M_PI / 2;
    for (size_t i = 1; i < m; ++i) {
      for (size_t j = 1; j < m; ++j) { o += coef[i][j] * sin(i * z.real()) * sin(j * z.imag()); }
    }
    return vb::Indexed (o > 0 ? 2 : 1);
  }

  public:
  Wave2(const std::string &t, size_t n, size_t m_) : vb::Coloring(t, cpx(0, 0), cpx(M_PI, M_PI), n, [&](cpx z) { return c(z); }), m(m_) {
    for (size_t i = 0; i < m; ++i) {
      coef.push_back({});
      for (size_t j = 0; j < m; ++j) { coef.back().push_back(vb::prng.gaussian() / sqrt(double(i)*i + double(j)*j + 1.0)); }
    }
  }
};

auto main(int argc, char **argv) -> int {
  vb::CLP clp(argc, argv, "Waves in a rectangle");
  auto    n = clp.param("n", size_t(800), "Size of the rectangle");
  auto    m = clp.param("m", size_t(150), "Number of frequencies");
  clp.finalize();

  Wave2 w(clp.title, n, m);
  w.show();
  w.pause();
}