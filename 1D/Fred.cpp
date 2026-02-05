#include <fstream>
#include <vb/ProgressBar.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

struct state {
    static inline double a, b;
    double               v, x;
    void                 step() {
        x = a * x + b;
        while (x >= 1) x -= 1;
    }
};

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
    vb::CLP clp(argc, argv, "Deterministic Fourier law");
    state::a = clp.param("a", 2.0, "A parameter");
    state::b = clp.param("b", 0.2394879347, "B parameter");
    auto nn  = clp.param("n", size_t(500), "System size");
    auto mm  = clp.param("m", size_t(1000), "Number of samples");
    auto tt  = clp.param("t", size_t(100), "Time steps");
    clp.finalize();

    size_t              nn2 = nn / 2;
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
        auto   nclass = size_t(sqrt(double(mm)));
        double bmin = 0, bmax = 0;
        for (auto b : boltzmann) {
            bmin = std::min(bmin, b);
            bmax = std::max(bmax, b);
        }
        std::vector<int> data(nclass);
        for (auto b : boltzmann) data[unsigned((double(nclass) - .01) * (b - bmin) / (bmax - bmin))]++;
        std::ofstream of("out.boltzmann");
        for (size_t i = 0; i < nclass; ++i) of << bmin + double(i) * (bmax - bmin) / double(nclass) << " " << data[i] << std::endl;
    }
}
