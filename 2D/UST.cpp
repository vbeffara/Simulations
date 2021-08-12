#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

enum Type { VOID, SITE, EDGE, EMPH };

class Point {
public:
    explicit Point(int /*unused*/ = 0) {}
    explicit Point(Type _t) : t(_t) {}
    explicit operator Color() {
        if ((t == SITE) || (t == EDGE)) return WHITE;
        if (t == EMPH) return RED;
        if (d < 0) return BLACK;
        return Indexed(int(d));
    }

    size_t d = size_t(-1);
    Type   t = VOID;
};

class UST : public Bitmap<Point> {
public:
    explicit UST(const Hub &H, size_t n_) : Bitmap<Point>(H.title, {2 * n_ + 1, 2 * n_ + 1}), n(n_) {}

    void path(ucoo z, Type tgt) {
        while (at(z).t != tgt) {
            coo d   = dz[at(z).d];
            at(z).t = at(z + d).t = tgt;
            z += d * 2;
        }
    }

    void lerw(ucoo z0) {
        auto z = z0;
        while (at(z).t != SITE) {
            auto d  = prng.uniform_int(4u);
            at(z).d = d;
            if (fits(z + dz[d])) z += dz[d] * 2;
            step();
        }
        path(z0, SITE);
    }

    void go(const Hub &H) {
        show();
        put({0, 2 * (n / 2)}, Point{SITE});
        for (size_t i = n + 1; i-- > 0;)
            for (size_t j = 0; j <= n; ++j) lerw({2 * i, 2 * j});
        if (H['p']) {
            put({0, 2 * (n / 2)}, Point{EMPH});
            path({2 * n, 2 * (n / 4)}, EMPH);
        }
        pause();
        output(H.title);
    }

    size_t n;
};

auto main(int argc, char **argv) -> int {
    Hub H("Uniform spanning tree", argc, argv, "n=200,p");
    UST(H, H['n']).go(H);
}
