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
        // TODO: coo_range
        for (size_t i = 0; i < size.x; ++i)
            for (size_t j = 0; j < size.y; ++j)
                if (prng.bernoulli(H['p'])) put({i, j}, prng.bernoulli(H['l']) ? 2 : 1);
    }
    void clean() {
        // TODO: coo_range
        for (size_t x = 0; x < size.x; ++x)
            for (size_t y = 0; y < size.y; ++y)
                if (at({x, y}) > 0) at({x, y}) = at({x, y}) + 100;
        bool dirty = true;
        while (dirty) {
            dirty = false;
            // TODO: coo_range
            for (int64_t x = 0; x < int64_t(size.x); ++x)
                for (int64_t y = 0; y < int64_t(size.y); ++y) {
                    if (at(ucoo(coo{x, y})) == 102) {
                        if (atp({x + 1, y}) == 1 || atp({x + 1, y}) == 2 || atp({x, y + 1}) == 1 || atp({x, y + 1}) == 2 ||
                            atp({x, y - 1}) == 1 || atp({x, y - 1}) == 2) {
                            atp({x, y}) = 2;
                            dirty       = true;
                        }
                    } else if (at(ucoo(coo{x, y})) == 101) {
                        if (atp({x + 1, y}) == 1 || atp({x + 1, y}) == 2 || atp({x + 1, y}) == 101 || atp({x, y + 1}) == 1 ||
                            atp({x, y + 1}) == 2 || atp({x, y - 1}) == 1 || atp({x, y - 1}) == 2) {
                            atp({x, y}) = 1;
                            dirty       = true;
                        }
                    }
                }
        }
        // TODO: coo_range
        for (size_t x = 0; x < size.x; ++x)
            for (size_t y = 0; y < size.y; ++y) {
                if (at({x, y}) >= 102) at({x, y}) = 3;
                if (at({x, y}) == 101) at({x, y}) = 1;
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
            // TODO: coo_range
            for (size_t x = 0; x < P.size.x; ++x)
                for (size_t y = 0; y < P.size.y; ++y)
                    if (P.at({x, y}) == 2) ++na;
            if (na == 0) exit(0);
            cout << t / (P.size.x * P.size.y) << " " << na << " " << P.flow.x << " " << double(P.flow.x) / na << endl;
            P.flow = {0, 0};
        }
        P.move();
    }
}
