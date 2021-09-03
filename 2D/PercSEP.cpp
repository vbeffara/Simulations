#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

enum state : size_t { s_closed, s_empty, s_full, s_stuck, s_empty_tagged, s_full_tagged };
auto tagged(state s) -> state { return s == s_empty ? s_empty_tagged : s == s_full ? s_full_tagged : s; }
auto untagged(state s) -> state { return s == s_empty_tagged ? s_empty : s == s_full_tagged ? s_full : s; }
auto not_stuck(state s) -> bool { return (s == s_empty) || (s == s_full) || (s == s_empty_tagged); }

template <> auto vb::to_Color(state t) -> Color {
    switch (t) {
    case s_empty: return Grey(90);
    case s_full: return GREEN;
    case s_stuck: return RED;
    default: return BLACK;
    }
}

class PercSEP : public Bitmap<state> {
public:
    coo    flow{0, 0};
    double drift;
    bool   tasym, stats;

    PercSEP(const std::string &title_, size_t sz, double d, bool t, double p, double l, bool s)
        : Bitmap<state>(title_, {2 * sz, sz}), drift(d), tasym(t), stats(s) {
        for (const auto &z : coo_range(size))
            if (prng.bernoulli(p)) put(z, prng.bernoulli(l) ? s_full : s_empty);
        show();
    }

    void clean() {
        for (const auto &z : coo_range(size)) at(z) = tagged(at(z));

        for (bool dirty = true; dirty;) {
            dirty = false;
            for (const auto &z : coo_range(coo(size))) {
                if ((atp(z) != s_full_tagged) && (atp(z) != s_empty_tagged)) continue;
                if (not_stuck(atp(z + coo{1, 0})) || not_stuck(atp(z + coo{0, 1})) || not_stuck(atp(z - coo{0, 1}))) {
                    atp(z) = untagged(atp(z));
                    dirty  = true;
                }
            }
        }

        for (const auto &z : coo_range(size)) {
            if (at(z) == s_full_tagged) at(z) = s_stuck;
            if (at(z) == s_empty_tagged) at(z) = s_empty;
        }
    }

    void move() {
        auto z = prng.uniform_coo(size);
        if (at(z) != s_full) return;
        coo s = prng.bernoulli(drift) ? coo{1, 0} : dz[prng.uniform_int(4U)];
        if (tasym && (s == coo{-1, 0})) return;
        if (atp(coo(z) + s) != s_empty) return;
        put(z, s_empty);
        putp(coo(z) + s, s_full);
        flow += s;
    }

    void run() {
        for (size_t t = 1;; ++t) {
            move();
            if ((t % (size.x * size.y)) != 0) continue;
            if (tasym) clean();
            if (!stats) continue;
            int na = 0;
            for (const auto &z : coo_range(size))
                if (at(z) == s_full) ++na;
            if (na == 0) return;
            fmt::print("{} {} {} {}\n", t / (size.x * size.y), na, flow.x, double(flow.x) / na);
            flow = {0, 0};
        }
    }
};

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "Exclusion on Percolation");
    auto d = clp.param("d", 0.000, "Particle drift");
    auto l = clp.param("l", 0.300, "Particle density relative to cluster");
    auto n = clp.param("n", 400UL, "Domain size");
    auto p = clp.param("p", 0.700, "Percolation parameter");
    auto T = clp.flag("t", "Run a TASEP (no left jump, trap coloring)");
    auto s = clp.flag("s", "Output statistics on particle movement");
    clp.finalize();

    PercSEP(clp.title, n, d, T, p, l, s).run();
}
