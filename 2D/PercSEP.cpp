#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

enum state : size_t { s_closed, s_empty, s_full, s_stuck };

template <> auto vb::to_Color(state t) -> Color {
    switch (t) {
    case s_closed: return BLACK;
    case s_empty: return Grey(90);
    case s_full: return GREEN;
    case s_stuck: return Color(128, 0, 0);
    }
}

class PercSEP : public Bitmap<state> {
public:
    coo    flow{0, 0};
    double drift;
    bool   tasym, stats;

    PercSEP(std::string title, size_t sz, double d, bool t, double p, double l, bool s)
        : Bitmap<state>(title, {2 * sz, sz}), drift(d), tasym(t), stats(s) {
        for (const auto &z : coo_range(size))
            if (prng.bernoulli(p)) put(z, prng.bernoulli(l) ? s_full : s_empty);
        show();
    }

    void clean() {
        for (const auto &z : coo_range(size))
            if (at(z) > 0) at(z) = state(at(z) + 100);
        bool dirty = true;

        while (dirty) {
            dirty = false;
            for (const auto &z : coo_range(coo(size))) {
                if (at(ucoo(z)) == 102) {
                    if (atp(z + coo{1, 0}) == 1 || atp(z + coo{1, 0}) == 2 || atp(z + coo{0, 1}) == 1 || atp(z + coo{0, 1}) == 2 ||
                        atp(z - coo{0, 1}) == 1 || atp(z - coo{0, 1}) == 2) {
                        atp(z) = s_full;
                        dirty  = true;
                    }
                } else if (at(ucoo(z)) == 101) {
                    if (atp(z + coo{1, 0}) == 1 || atp(z + coo{1, 0}) == 2 || atp(z + coo{1, 0}) == 101 || atp(z + coo{0, 1}) == 1 ||
                        atp(z + coo{0, 1}) == 2 || atp(z - coo{0, 1}) == 1 || atp(z - coo{0, 1}) == 2) {
                        atp(z) = s_empty;
                        dirty  = true;
                    }
                }
            }
        }

        for (const auto &z : coo_range(size)) {
            if (at(z) >= 102) at(z) = s_stuck;
            if (at(z) == 101) at(z) = s_empty;
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

    void run() {
        for (size_t t = 1;; ++t) {
            if (stats && (t % (size.x * size.y)) == 0) {
                if (tasym) clean();
                int na = 0;
                for (const auto &z : coo_range(size))
                    if (at(z) == 2) ++na;
                if (na == 0) return;
                fmt::print("{} {} {} {}\n", t / (size.x * size.y), na, flow.x, double(flow.x) / na);
                flow = {0, 0};
            }
            move();
        }
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

    PercSEP(clp.title, n, d, T, p, l, s).run();
}
