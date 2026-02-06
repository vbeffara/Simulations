#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;

const Color none(BLACK), prey(GREEN), pred(RED);

class SIR : public Image {
public:
    SIR(const std::string &title, size_t n, double l_, bool diag) : Image(title, {n, n}), l(l_), d(diag) {
        size_t const n0 = d ? 0 : n / 2;
        for (size_t ii = n0 - 10; ii < n0 + 10; ++ii)
            for (size_t j = n0 - 10; j < n0 + 10; ++j) {
                ucoo const z{ii, j};
                if (fits(z)) put(z, prey);
            }
        put({n0, n0}, pred);
        for (auto z : coo_range(size))
            if (at(z) == prey) fringe.push_back(z);
        show();
    }

    void go() {
        auto ii = prng.uniform_int(fringe.size());
        auto z  = fringe[ii];
        auto nz = z + dz[prng.uniform_int(d ? 2U : 4U)];
        if (!fits(nz)) return;
        if ((at(nz) == none) && ((l > 1) || prng.bernoulli(l))) {
            put(nz, prey);
            fringe.push_back(nz);
        }
        if ((at(nz) == pred) && ((l < 1) || prng.bernoulli(1 / l))) {
            put(z, pred);
            fringe[ii] = fringe.back();
            fringe.pop_back();
        }
    }

    double            l;
    bool              d;
    std::vector<ucoo> fringe;
};

auto main(int argc, char **argv) -> int {
    CLP clp(argc, argv, "SIR process on the lattice");
    auto n = clp.param("n", size_t(600), "Grid size");
    auto l = clp.param("l", 0.5, "Infection rate");
    auto d = clp.flag("d", "Diagonal mode");
    auto s = clp.param("s", 1, "Update interval");
    clp.finalize();

    SIR img(clp.title, n, l, d);
    for (int t = 0; !img.fringe.empty(); ++t) {
        img.go();
        if (t % s == 1) img.update();
    }
    img.output(clp.title);
}
