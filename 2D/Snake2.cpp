#include <vb/Bitmap.h>
#include <vb/Ranges.h>

using namespace vb;

enum site { EMPTY = 0, VERTEX, EDGE, WORK, LEFT, RIGHT };

namespace vb {
    template <> Color to_Color(site s) {
        switch (s) {
        case EMPTY: return WHITE;
        case VERTEX: return BLACK;
        case EDGE: return Grey(128);
        case WORK: return RED;
        case LEFT: return Indexed(1);
        case RIGHT: return Indexed(2);
        }
    }
} // namespace vb

class Snake : public Bitmap<site> {
public:
    Snake(int n, double l) : Bitmap(4 * n + 1, 2 * n + 1), lambda(l), path(1, {2 * n, 0}) {
        for (int x = 0; x < w(); ++x) put({x, 0}, VERTEX);
        if (H['d']) triangle();
        if (!H['v']) show();
    }

    void triangle() {
        for (int y = 0; y < h(); ++y) {
            int yy = y + (y % 2);
            int s  = 2 + yy / sqrt(3);
            s += (s % 2);
            for (int x = w() / 2 + s; x < w(); ++x) put({x, y}, VERTEX);
            for (int x = w() / 2 - s; x >= 0; --x) put({x, y}, VERTEX);
        }
    }

    bool border(coo z) { return (z.x == 0) || (z.x == w() - 1) || (z.y == h() - 1); }

    bool trapped(coo z, int d) {
        if (border(z)) return false;
        bool surrounded = true;
        for (int i = 0; i < 4; ++i)
            if (at(z + dz[i] * 2) == EMPTY) surrounded = false;
        if (surrounded) return true;

        d = (d + 1) % 4;
        while (at(z + dz[d] * 2) == VERTEX) d = (d + 3) % 4;
        coo zz = z;
        int dd = d;
        while (true) {
            zz += dz[d] * 2;
            if (border(zz)) return false;
            d = (d + 1) % 4;
            while (at(zz + dz[d] * 2) == VERTEX) d = (d + 3) % 4;
            if ((zz == z) && (dd == d)) return true;
        }
    }

    bool allowed(coo z, int d) {
        coo nz = z + dz[d] * 2;
        if (at(nz) == VERTEX) return false;
        if (nz.y < 0) return false;
        if (trapped(nz, d)) return false;
        return true;
    }

    void run() {
        while (true) {
            if (prng.bernoulli(1 / (1 + 4 * lambda))) {
                if (path.size() > 1) {
                    coo z = path.back();
                    path.pop_back();
                    put(z, EMPTY);
                    put((z + path.back()) / 2, EMPTY);
                }
                continue;
            }
            coo z = path.back();
            int d = prng.uniform_int(4);
            if (!allowed(z, d)) continue;
            coo nz = z + dz[d] * 2;
            put(z + dz[d], EDGE);
            put(nz, VERTEX);
            path.push_back(nz);
            if (border(nz)) break;
        }
    }

    double           lambda;
    std::vector<coo> path;
};

int main(int argc, char ** argv) {
    H.init("Self-avoiding snake in the half plane", argc, argv, "n=200,l=1.0,d,v,p");
    Snake S(H['n'], H['l']);
    S.run();
    S.fill({S.w() / 2 + 1, 1}, RIGHT);
    S.fill({S.w() / 2 - 1, 1}, LEFT);
    if (S.visible()) {
        if (H['p']) S.pause();
        S.output();
    }
    H.output("Final value of x", "x", S.path.back().x);
}
