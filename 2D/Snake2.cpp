#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

enum site { EMPTY = 0, VERTEX, EDGE, LEFT, RIGHT };

namespace vb {
    template <> Color to_Color(site t) {
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
        for (size_t x = 0; x < size_t(w()); ++x) put({x, 0u}, VERTEX);
        if (double a = H['a']; a != 0) triangle(a);
        if (!H['v']) show();
    }

    void triangle(double a) {
        double ta = tan(a * M_PI / 180);
        for (size_t y = 0; y < size_t(h()); ++y) {
            auto yy = y + (y % 2);
            auto s  = int(2 + yy * ta);
            s += (s % 2);
            for (size_t x = 0; x < size_t(w()); ++x)
                if (abs(int(x) - w() / 2) >= s) put({x, y}, VERTEX);
        }
    }

    bool border(coo z) { return (z.x == 0) || (z.x == w() - 1) || (z.y == h() - 1); }

    bool edge(coo z, int d) { return (d != 3 - ((z.x + z.y + w() / 2) % 4)); }

    bool trapped(coo z, int d) {
        if (border(z)) return false;
        bool surrounded = true;
        for (int i = 0; i < 4; ++i)
            if ((at(ucoo(z + dz[i] * 2)) == EMPTY) && (!hex || edge(z, i))) surrounded = false;
        if (surrounded) return true;

        d = (d + 1) % 4;
        while ((at(ucoo(z + dz[d] * 2)) == VERTEX) || (hex && !edge(z, d))) d = (d + 3) % 4;
        coo zz = z;
        int dd = d;
        while (true) {
            zz += dz[d] * 2;
            if (border(zz)) return false;
            d = (d + 1) % 4;
            while ((at(ucoo(zz + dz[d] * 2)) == VERTEX) || (hex && !edge(zz, d))) d = (d + 3) % 4;
            if ((zz == z) && (dd == d)) return true;
        }
    }

    bool allowed(coo z, int d) {
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
            int d = prng.uniform_int(4);
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

int main(int argc, char **argv) {
    Hub   H("Self-avoiding snake in the half plane", argc, argv, "n=200,l=1.0,v,p,x,a=0");
    Snake S(H, H['n'], H['l'], H['x']);
    S.run();
    S.fill({size_t(S.w() / 2 + 1), 1}, RIGHT);
    S.fill({size_t(S.w() / 2 - 1), 1}, LEFT);
    if (S.visible()) {
        if (H['p']) S.pause();
        S.output(H.title);
    }
    H.output("Final value of x", "x", S.path.back().x);
}
