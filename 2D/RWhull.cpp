#include <vb/Bitmap.h>
#include <vb/ProgressBar.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace std;
using namespace vb;

auto bridge(size_t n, bool p = false) -> vector<int> {
    vector<int> v(n, -1);
    for (size_t i = 0; i < n / 2; ++i) v[i] = 1;
    for (size_t i = n; i-- > 0;) swap(v[i], v[prng.uniform_int(i + 1)]);

    if (!p) return v;

    int    s = 0, ms = 0;
    size_t mi = 0;
    for (size_t i = 0; i < n; ++i) {
        s += v[i];
        if (s < ms) {
            ms = s;
            mi = i;
        }
    }

    vector<int> w;
    for (size_t i = mi + 1; i < n; ++i) w.push_back(v[i]);
    for (size_t i = 0; i <= mi; ++i) w.push_back(v[i]);
    return w;
}

int m = 1;

namespace vb {
    template <> auto to_Color(int t) -> Color {
        if (t == 0) return WHITE;
        if (t == 1) return RED;
        return Grey(uint8_t(215 - (t * 215) / m));
    }
} // namespace vb

class Snake : public Bitmap<int> {
public:
    Snake(const Hub &H, size_t n_) : Bitmap<int>(H.title, {6 * n_, 6 * n_}), n(n_) {
        p.push_back({3 * n, 3 * n});
        put(p.back(), 1);
        show();
    };
    void grow(unsigned d) {
        p.push_back(p.back() + dz[d]);
        put(p.back(), at(p.back()) + 1);
    }
    void shrink() { p.pop_back(); }

    size_t       n;
    vector<ucoo> p;
};

auto main(int argc, char **argv) -> int {
    Hub    H("Random walk hull", argc, argv, "n=50,i,v");
    size_t n = H['n'], l = n * n * n * n;
    bool   inf = H['i'], vid = H['v'];

    Snake S(H, n);

    if (inf) {
        if (vid) S.snapshot_setup("RWSH", 1.0);
        while (size_t(norm(S.p.back() - coo{3 * int(n), 3 * int(n)})) < (3 * n - 1) * (3 * n - 1)) {
            if ((S.p.size() == 1) || (prng.bernoulli(.5)))
                S.grow(prng.uniform_int(4U));
            else
                S.shrink();
        }
    } else {
        auto b = bridge(l, true);
        for (size_t i = 0; i < l; ++i) {
            if (b[i] == 1)
                S.grow(prng.uniform_int(4U));
            else
                S.shrink();
        }
    }

    S.pause();
}
