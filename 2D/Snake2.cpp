#include <vb/Bitmap.h>

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
    Snake(int n, double l, bool t) : Bitmap(4 * n + 1, 2 * n + 1), lambda(l), t(t), path(1, {2 * n, 0}) {
        for (int x = 0; x < w(); ++x) put({x, 0}, VERTEX);
        show();
    }

    bool trapped(coo z) {
        std::vector<coo> q(1, z), cc(0);
        bool             ans = true;
        while (q.size() > 0) {
            coo z = q.back();
            q.pop_back();
            if (at(z) == EMPTY) {
                at(z) = WORK;
                cc.push_back(z);
                for (int i = 0; i < 4; ++i) {
                    coo nz = z + dz[i] * 2;
                    if (!contains(nz)) {
                        ans = false;
                        break;
                    }
                    if (at(nz) == EMPTY) q.push_back(nz);
                }
            }
        }
        for (auto z : cc) at(z) = EMPTY;
        return ans;
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
            coo z = path.back(), d = dz[prng.uniform_int(4)], nz = z + d * 2;
            if ((at(nz) == VERTEX) || (nz.y < 0)) continue;
            if (!contains(nz)) break;
            if (t && trapped(nz)) continue;
            put(z + d, EDGE);
            put(nz, VERTEX);
            if ((nz.x == 0) || (nz.x == w() - 1) || (nz.y == h() - 1)) break;
            path.push_back(nz);
        }
    }

    double           lambda;
    bool             t;
    std::vector<coo> path;
};

int main(int argc, char ** argv) {
    H.init("Self-avoiding snake in the half plane", argc, argv, "n=200,l=1.0,t");
    Snake S(H['n'], H['l'], H['t']);
    S.run();
    S.fill({S.w() / 2 + 1, 1}, RIGHT);
    S.fill({S.w() / 2 - 1, 1}, LEFT);
    S.output();
}
