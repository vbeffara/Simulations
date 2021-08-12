#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

enum site { EMPTY = 0, VERTEX, EDGE, LEFT, RIGHT };

namespace vb {
    template <> auto to_Color(site t) -> Color {
        if (t == VERTEX) return BLACK;
        if (t == EDGE) return Grey(128);
        if (t == LEFT) return Indexed(1);
        if (t == RIGHT) return Indexed(2);
        return WHITE;
    }
} // namespace vb

class Snake : public Bitmap<site> {
public:
    Snake(const Hub &H, size_t n, double l, bool hex)
        : Bitmap(H.title, {4 * n + 1, 2 * n + 1}), lambda(l), path(1, {2 * int(n), 0}), hex(hex) {
        for (size_t x = 0; x < size.x; ++x) put({x, 0U}, VERTEX);
        if (double a = H['a']; a != 0) triangle(a);
        if (!H['v']) show();
    }

    void triangle(double a) {
        double ta = tan(a * M_PI / 180);
        for (size_t y = 0; y < size.y; ++y) {
            auto yy = y + (y % 2);
            auto s  = 2 + int(double(yy) * ta);
            s += (s % 2);
            for (size_t x = 0; x < size.x; ++x)
                if (abs(int(x) - int(size.x / 2)) >= s) put({x, y}, VERTEX);
        }
    }

    // TODO: now remove all int(size.y), int64_t(size.y), int(size.x), int64_t(size.x)
    auto border(coo z) -> bool { return (z.x == 0) || (z.x == int(size.x) - 1) || (z.y == int(size.y) - 1); }

    auto edge(coo z, size_t d) -> bool { return (d != 3 - ((unsigned(z.x + z.y) + size.x / 2) % 4)); }

    auto trapped(coo z, size_t d) -> bool {
        if (border(z)) return false;
        bool surrounded = true;
        for (unsigned i = 0; i < 4; ++i)
            if ((at(ucoo(z + dz[i] * 2)) == EMPTY) && (!hex || edge(z, i))) surrounded = false;
        if (surrounded) return true;

        d = (d + 1) % 4;
        while ((at(ucoo(z + dz[d] * 2)) == VERTEX) || (hex && !edge(z, d))) d = (d + 3) % 4;
        coo zz = z;
        auto dd = d;
        while (true) {
            zz += dz[d] * 2;
            if (border(zz)) return false;
            d = (d + 1) % 4;
            while ((at(ucoo(zz + dz[d] * 2)) == VERTEX) || (hex && !edge(zz, d))) d = (d + 3) % 4;
            if ((zz == z) && (dd == d)) return true;
        }
    }

    auto allowed(coo z, size_t d) -> bool {
        if (hex && !edge(z, d)) return false;
        coo nz = z + dz[d] * 2;
        return (at(ucoo(nz)) != VERTEX) && (nz.y >= 0) && !trapped(nz, d);
    }

    void run() {
        while (true) {
            if (prng.bernoulli(1 / (1 + 4 * lambda))) {
                if (path.size() > 1) {
                    coo z = path.back();
                    path.pop_back();
                    put(ucoo(z), EMPTY);
                    put(ucoo((z + path.back()) / 2), EMPTY);
                }
                continue;
            }
            coo z = path.back();
            auto d = prng.uniform_int(4u);
            if (!allowed(z, d)) continue;
            coo nz = z + dz[d] * 2;
            put(ucoo(z + dz[d]), EDGE);
            put(ucoo(nz), VERTEX);
            if (border(nz)) break;
            path.push_back(nz);
        }
    }

    double           lambda;
    std::vector<coo> path;
    bool             hex;
};

auto main(int argc, char **argv) -> int {
    Hub   H("Self-avoiding snake in the half plane", argc, argv, "n=200,l=1.0,v,p,x,a=0");
    Snake S(H, H['n'], H['l'], H['x']);
    S.run();
    S.fill({S.size.x / 2 + 1, 1}, RIGHT);
    S.fill({S.size.x / 2 - 1, 1}, LEFT);
    if (S.visible()) {
        if (H['p']) S.pause();
        S.output(H.title);
    }
    H.output("Final value of x", "x", S.path.back().x);
}
