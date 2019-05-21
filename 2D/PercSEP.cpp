#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

namespace vb {
    template <> Color to_Color(int t) {
        static const vector<Color> C{BLACK, Grey(90), GREEN, Color(128, 0, 0)};
        return C[size_t(t)];
    }
} // namespace vb

class PercSEP : public Bitmap<int> {
public:
    explicit PercSEP(const Hub &H) : Bitmap<int>(H.title, {2 * size_t(H['n']), H['n']}), flow({0, 0}), d(H['d']), tasym(H['t']) {
        for (const auto &z : coo_range(size))
            if (prng.bernoulli(H['p'])) put(z, prng.bernoulli(H['l']) ? 2 : 1);
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
        coo s = prng.bernoulli(d) ? coo{1, 0} : dz[prng.uniform_int(4)];
        if (atp(coo(z) + s) != 1) return;
        if (tasym && (s == coo{-1, 0})) return;
        swap(at(z), atp(coo(z) + s));
        step();
        flow += s;
    }
    coo    flow;
    double d;
    bool   tasym;
};

int main(int argc, char **argv) {
    Hub     H("Exclusion on Percolation", argc, argv, "n=400,p=.8,l=.3,d=0,t");
    PercSEP P(H);
    P.show();

    for (size_t t = 1;; ++t) {
        if ((t % (P.size.x * P.size.y)) == 0) {
            if (P.tasym) P.clean();
            int na = 0;
            for (const auto &z : coo_range(P.size))
                if (P.at(z) == 2) ++na;
            if (na == 0) exit(0);
            cout << t / (P.size.x * P.size.y) << " " << na << " " << P.flow.x << " " << double(P.flow.x) / na << endl;
            P.flow = {0, 0};
        }
        P.move();
    }
}
