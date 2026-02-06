#include <queue>
#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

namespace vb {
    template <> auto to_Color(unsigned t) -> Color {
        static const vector<Color> C = {BLACK, RED, GREEN};
        return C[t];
    }
} // namespace vb

class Particle {
public:
    explicit Particle(ucoo xy = {0, 0}, unsigned s = 0, bool z = false, double t = 0) : state(s), location(xy), type(z), next(t){};
    auto operator<(const Particle &o) const -> bool { return next > o.next; }

    [[nodiscard]] auto jump() const -> coo {
        if (type) {
            auto out = prng.uniform_int(2U);
            if (state == 2) out += 2;
            return dz[out];
        }
        auto out = prng.uniform_int(4U);
        if (out + 2 * state == 4) out = 2 - out;
        return dz[out];
    }

    unsigned state;
    ucoo     location;
    bool     type;
    double   next;
};

class Sagex : public Bitmap<unsigned> {
public:
    Sagex(const string &title, size_t w, size_t h, double lambda, double pp, bool zz)
        : Bitmap<unsigned>(title, {w, h}), vid(false) {
        for (size_t x = 0; x < w; ++x)
            for (size_t y = 0; y < h; ++y) {
                if (prng.bernoulli(lambda)) {
                    Particle const p({x, y}, prng.bernoulli(pp) ? 1 : 2, zz, prng.exponential());
                    put({x, y}, p.state);
                    q.push(p);
                }
            }
    }

    void go() {
        show();
        if (vid) snapshot_setup("Sagex", 1);
        while (true) {
            Particle p = q.top();
            q.pop();
            auto nz = wrap(p.jump() + p.location, size);
            if (at(nz) == 0) {
                swap(at(p.location), at(nz));
                step();
                p.location = nz;
            }
            p.next += prng.exponential();
            q.push(p);
        }
    }

    priority_queue<Particle> q;
    bool                     vid;
};

auto main(int argc, char **argv) -> int {
    CLP clp(argc, argv, "Sagex process");
    auto n      = clp.param("n", 400, "Grid height");
    auto w      = clp.param("w", 0, "Grid width (0 for same as height)");
    auto lambda = clp.param("l", 0.22, "Particle density");
    auto pp     = clp.param("p", 0.5, "Probability of state 1");
    auto vid    = clp.flag("v", "Video output");
    auto zz     = clp.flag("z", "Particle type flag");
    clp.finalize();

    Sagex S(clp.title, w != 0 ? size_t(w) : size_t(n), size_t(n), lambda, pp, zz);
    S.vid = vid;
    S.go();
}
