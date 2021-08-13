#include <vb/Coloring.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using vb::cpx;
using vb::Hub;
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
        auto operator()(const cpx &z) -> double { return a * cos(c * real(z) + s * imag(z) + p); }
        double a, c, s, p;
    };

    Wave(const vb::Hub &H, size_t n, int k, double l_, double w_, double e_)
        : Coloring(H.title, cpx(-double(n), -double(n)), cpx(double(n), double(n)), n, [&](cpx z) { return c(z); }), H(H) {
        for (int i = 0; i < k; ++i) {
            double delta     = (2 * prng.uniform_real() - 1) * (1 + e_);
            double amplitude = bump(delta, e_) * prng.gaussian();
            m.emplace_back(amplitude, prng.uniform_real(0, 2.0 * M_PI), l_ * (1 + w_ * delta), prng.uniform_real(0, 2.0 * M_PI));
        }
        detail = 2.0 / l_;
    }

    auto v(cpx z) -> double {
        double out = double(H['t']) * real(z);
        for (auto mm : m) { out += mm(z); }
        return out;
    }

    auto c(cpx z) -> vb::Color { return vb::Indexed(v(z) > 0 ? 1 : 2); }

    std::vector<Mode> m;
    const vb::Hub &   H;
};

auto main(int argc, char **argv) -> int {
    Hub H("Random planar waves", argc, argv, "n=600,k=1000,l=.2,w=0,e=0,s=0,t=0");
    if (size_t s = H['s']) { prng.seed(s); }
    Wave W(H, H['n'], H['k'], H['l'], H['w'], H['e']);
    W.show();
    W.output(H.title);
    Fl::run();
}
