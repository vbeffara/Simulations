#include <vb/Coloring.h>
#include <vb/util/PRNG.h>

using vb::cpx;
using vb::H;
using vb::prng;

double f(double x) { return exp(-1 / x); }
double g(double x) { return f(1 - x) / (f(x) + f(1 - x)); }

double bump(double x, double e) {
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
        double operator()(const cpx &z) { return a * cos(c * real(z) + s * imag(z) + p); }
        double a, c, s, p;
    };

    Wave(int n, int k, double l_, double w_, double e_) : Coloring(cpx(-n, -n), cpx(n, n), n, [&](cpx z) { return c(z); }) {
        for (int i = 0; i < k; ++i) {
            double delta     = (2 * prng.uniform_real() - 1) * (1 + e_);
            double amplitude = bump(delta, e_) * prng.gaussian();
            m.emplace_back(amplitude, prng.uniform_real(0, 2.0 * M_PI), l_ * (1 + w_ * delta), prng.uniform_real(0, 2.0 * M_PI));
        }
        detail = 2.0 / l_;
    }

    double v(cpx z) {
        double out = double(H['t']) * real(z);
        for (auto mm : m) { out += mm(z); }
        return out;
    }

    vb::Color c(cpx z) { return vb::Indexed(v(z) > 0 ? 1 : 2); }

    std::vector<Mode> m;
};

int main(int argc, char **argv) {
    H.init("Random planar waves", argc, argv, "n=600,k=1000,l=.2,w=0,e=0,s=0,t=0");
    if (int s = H['s']) { prng.seed(s); }
    Wave W(H['n'], H['k'], H['l'], H['w'], H['e']);
    W.show();
    W.output();
    Fl::run();
}
