#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

namespace vb {
    template <> auto to_Color(int t) -> Color {
        static const vector<Color> C{BLACK, Grey(90), GREEN, Color(128, 0, 0)};
        return C[size_t(t)];
    }
} // namespace vb

class PercSEP : public Bitmap<int> {
public:
    coo    flow{0, 0};
    double drift;
    bool   tasym;

    PercSEP(std::string title, size_t sz, double d, bool t, double p, double l) : Bitmap<int>(title, {2 * sz, sz}), drift(d), tasym(t) {
        for (const auto &z : coo_range(size))
            if (prng.bernoulli(p)) put(z, prng.bernoulli(l) ? 2 : 1);
    }

    void clean() {
        for (const auto &z : coo_range(size))
            if (at(z) > 0) at(z) = at(z) + 100;
        bool dirty = true;

        while (dirty) {
            dirty = false;
            for (const auto &z : coo_range(coo(size))) {
                if (at(ucoo(z)) == 102) {
                    if (atp(z + coo{1, 0}) == 1 || atp(z + coo{1, 0}) == 2 || atp(z + coo{0, 1}) == 1 || atp(z + coo{0, 1}) == 2 ||
                        atp(z - coo{0, 1}) == 1 || atp(z - coo{0, 1}) == 2) {
                        atp(z) = 2;
                        dirty  = true;
                    }
                } else if (at(ucoo(z)) == 101) {
                    if (atp(z + coo{1, 0}) == 1 || atp(z + coo{1, 0}) == 2 || atp(z + coo{1, 0}) == 101 || atp(z + coo{0, 1}) == 1 ||
                        atp(z + coo{0, 1}) == 2 || atp(z - coo{0, 1}) == 1 || atp(z - coo{0, 1}) == 2) {
                        atp(z) = 1;
                        dirty  = true;
                    }
                }
            }
        }

        for (const auto &z : coo_range(size)) {
            if (at(z) >= 102) at(z) = 3;
            if (at(z) == 101) at(z) = 1;
        }
    }

    void move() {
        auto z = prng.uniform_coo(size);
        if (at(z) < 2) return;
        coo s = prng.bernoulli(drift) ? coo{1, 0} : dz[prng.uniform_int(4)];
        if (atp(coo(z) + s) != 1) return;
        if (tasym && (s == coo{-1, 0})) return;
        swap(at(z), atp(coo(z) + s));
        step();
        flow += s;
    }
};

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "Exclusion on Percolation");
    auto d = clp.param("d", 0.000, "Particle drift");
    auto l = clp.param("l", 0.300, "Particle density relative to cluster");
    auto n = clp.param("n", 400ul, "Domain size");
    auto p = clp.param("p", 0.700, "Percolation parameter");
    auto T = clp.flag("t", "Run a TASEP (no left jump, trap coloring)");
    auto s = clp.flag("s", "Output statistics on particle movement");
    clp.finalize();

    PercSEP P(clp.title, n, d, T, p, l);
    P.show();

    for (size_t t = 1;; ++t) {
        if (s && (t % (P.size.x * P.size.y)) == 0) {
            if (P.tasym) P.clean();
            int na = 0;
            for (const auto &z : coo_range(P.size))
                if (P.at(z) == 2) ++na;
            if (na == 0) exit(0);
            fmt::print("{} {} {} {}\n", t / (P.size.x * P.size.y), na, P.flow.x, double(P.flow.x) / na);
            P.flow = {0, 0};
        }
        P.move();
    }
}
