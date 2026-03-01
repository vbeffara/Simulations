#include <vb/Coloring.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using vb::cpx;
using vb::CLP;
using vb::prng;

auto f(double x) -> double { return exp(-1 / x); }
auto g(double x) -> double { return f(1 - x) / (f(x) + f(1 - x)); }

auto bump(double x, double e) -> double {
    if (x < 0) { x = -x; }
    if (x <= 1 - e) { return 1; }
    if (x >= 1 + e) { return 0; }
    return g((1 + (x - 1) / e) / 2);
}

class Wave : public vb::Coloring {
public:
    class Mode {
    public:
        Mode(double a_, double t, double l, double p_) : a(a_), c(l * cos(t)), s(l * sin(t)), p(p_) {}
        auto   operator()(const cpx &z) const -> double { return a * cos(c * real(z) + s * imag(z) + p); }
        double a, c, s, p;
    };

    Wave(const std::string &title, size_t n, int k, double l_, double ww_, double e_, double tilt_)
        : Coloring(title, cpx(-double(n), -double(n)), cpx(double(n), double(n)), n, [&](cpx z) { return c(z); }), tilt(tilt_) {
        for (int ii = 0; ii < k; ++ii) {
            double const delta     = (2 * prng.uniform_real() - 1) * (1 + e_);
            double const amplitude = bump(delta, e_) * prng.gaussian();
            m.emplace_back(amplitude, prng.uniform_real(0, 2.0 * M_PI), l_ * (1 + ww_ * delta), prng.uniform_real(0, 2.0 * M_PI));
        }
        detail = 2.0 / l_;
    }

    auto v(cpx z) -> double {
        double out = tilt * real(z);
        for (auto mm : m) { out += mm(z); }
        return out;
    }

    auto c(cpx z) -> vb::Color { return vb::Indexed(v(z) > 0 ? 1 : 2); }

    std::vector<Mode> m;
    double            tilt;
};

auto main(int argc, char **argv) -> int {
    CLP clp(argc, argv, "Random planar waves");
    auto n = clp.param("n", size_t(600), "Grid size");
    auto k = clp.param("k", 1000, "Number of modes");
    auto l = clp.param("l", 0.2, "Wavelength");
    auto w = clp.param("w", 0.0, "Width parameter");
    auto e = clp.param("e", 0.0, "Epsilon parameter");
    auto s = clp.param("s", size_t(0), "Random seed");
    auto t = clp.param("t", 0.0, "Tilt parameter");
    clp.finalize();

    if (s) { prng.seed(s); }
    Wave W(clp.title, n, k, l, w, e, t);
    W.show();
    W.output(clp.title);
    Fl::run();
}
