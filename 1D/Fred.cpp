#include <fstream>
#include <vb/ProgressBar.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

struct state {
    static double a, b;
    double        v, x;
    void          step() {
        x = a * x + b;
        while (x >= 1) x -= 1;
    }
};

double state::a, state::b;

class Model : public std::vector<state> {
public:
    explicit Model(size_t n) : std::vector<state>(n, {0, 0}) {
        for (auto &s : *this) s.x = vb::prng.uniform_real();
    }

    void rotate(size_t k, double a) {
        double c = cos(a), s = sin(a), &x = at(k).v, &y = at((k + 1) % size()).v;
        std::tie(x, y) = std::make_tuple(c * x - s * y, s * x + c * y);
    }

    void step(size_t k) {
        rotate(k, at(k).x * 2 * M_PI);
        at(k).step();
    }

    void swipe() {
        for (size_t k = 0; k < size(); k += 2) step(k);
        for (size_t k = 1; k < size(); k += 2) step(k);
    }
};

auto main(int argc, char **argv) -> int {
    vb::Hub H("Deterministic Fourier law", argc, argv, "a=2,b=.2394879347,n=500,m=1000,t=100");
    size_t  nn = H['n'], nn2 = nn / 2, mm = H['m'], tt = H['t'];
    state::a = H['a'];
    state::b = H['b'];
    std::vector<double> profile(nn, 0), boltzmann(mm, 0), var(tt, 0);

    vb::ProgressBar PB(mm);
    for (size_t i = 0; i < mm; ++i) {
        Model M(nn);
        M[nn2].v = 1;
        for (size_t t = 0; t < tt; ++t) {
            PB.set(i);
            M.swipe();
            double v = 0;
            for (size_t k = 0; k < nn; ++k) v += double((k - nn2) * (k - nn2)) * M[k].v * M[k].v;
            var[t] += v;
        }
        for (size_t k = 0; k < nn; ++k) profile[k] += M[k].v * M[k].v;
        boltzmann[i] = M[nn / 2].v;
    }

    {
        std::ofstream of("out.profile");
        for (auto u : profile) of << u << std::endl;
    }
    {
        std::ofstream of("out.variance");
        for (auto u : var) of << u << std::endl;
    }
    {
        auto   nclass = size_t(sqrt(double(H['m'])));
        double bmin = 0, bmax = 0;
        for (auto b : boltzmann) {
            bmin = std::min(bmin, b);
            bmax = std::max(bmax, b);
        }
        std::vector<int> data(nclass);
        for (auto b : boltzmann) data[unsigned((nclass - .01) * (b - bmin) / (bmax - bmin))]++;
        std::ofstream of("out.boltzmann");
        for (size_t i = 0; i < nclass; ++i) of << bmin + i * (bmax - bmin) / nclass << " " << data[i] << std::endl;
    }
}
