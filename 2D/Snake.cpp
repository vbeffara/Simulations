#include <vb/CoarseImage.h>
#include <vb/Path.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

class Snake : public vb::CoarseImage {
public:
    explicit Snake(const vb::Hub &H, size_t n)
        : vb::CoarseImage(H.title, {2 * n, 2 * n}, size_t(pow(n, .333))), z(1, vb::coo(vb::ucoo{n, n})) {
        put({n, n}, true);
    }

    void step(vb::coo dz) {
        auto nz = vb::ucoo(z.back()) + dz;
        if (at(nz)) return;
        z.push_back(vb::coo(nz));
        put(nz, true);
    }

    void shrink() {
        if (z.size() > 1) {
            put(vb::ucoo(z.back()), false);
            z.pop_back();
        }
    }

    [[nodiscard]] auto alive() const -> bool {
        auto lz = z.back();
        auto lx = lz.x, ly = lz.y;
        return ((lx > 0) && (lx < int64_t(size.x) - 1) && (ly > 0) && (ly < int64_t(size.y) - 1));
    }

    using vb::CoarseImage::output;
    void output(const std::string &s) override {
        vb::OldPath P(z.size() - 1);
        for (size_t i = 0; i < z.size() - 1; ++i) {
            auto dz = z[i + 1] - z[i];
            auto dx = dz.x, dy = dz.y;
            if (dx > 0) { P[i] = 0; }
            if (dx < 0) { P[i] = 2; }
            if (dy > 0) { P[i] = 1; }
            if (dy < 0) { P[i] = 3; }
        }
        P.output(s);
    }

    // TODO: vector<ucoo>
    std::vector<vb::coo> z;
};

auto main(int argc, char **argv) -> int {
    vb::Hub H("Self-avoiding snake", argc, argv, "n=1000,a=.38");
    double  a = H['a'], e = 1.0 / (1.0 + 4 * a);

    Snake S(H, H['n']);
    S.show();

    while (S.alive()) {
        if (vb::prng.bernoulli(e)) {
            S.shrink();
        } else {
            S.step(vb::dz[vb::prng.uniform_int(4)]);
        }
    }
    S.output(H.title);
    return 0;
}
