#include <spdlog/spdlog.h>
#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;

template <> auto vb::to_Color(double t) -> Color {
    if (t < -1e-9) return HSV(.5, t / (t - 1), 1);
    if (t > +1e-9) return HSV(0, t / (1 + t), 1);
    return BLACK;
}

class Sandpile : public Bitmap<double> {
public:
    Sandpile(const std::string &title, size_t n, double m, double s) : Bitmap(title, {n, n}) {
        for (auto z : coo_range(size)) put(z, prng.gaussian(m, s));
    }

    void swipe(bool r) {
        for (auto z : coo_range(size))
            if (double const excess = at(z); excess > 0) {
                for (unsigned ii = 0; ii < 4; ++ii) atp(coo(z) + dz[ii]) += excess / 4;
                at(z) = 0;
            }
        if (r) {
            double m = 0;
            for (auto z : coo_range(size))
                if (double const mm = abs(at(z)); mm > m) m = mm;
            if (m > 0)
                for (auto z : coo_range(size)) at(z) = at(z) * (100 / m);
        }
        update();
    }
};

auto main(int argc, char **argv) -> int {
    CLP clp(argc, argv, "Divisible sandpile");
    auto n = clp.param("n", size_t(500), "Grid size");
    auto m = clp.param("m", 0.01, "Mean of initial Gaussian");
    auto s = clp.param("s", 10.0, "Std dev of initial Gaussian");
    auto r = clp.flag("r", "Rescale after each step");
    clp.finalize();
    Sandpile S(clp.title, n, m, s);
    S.show();
    while (true) { S.swipe(r); }
}
