#include <vb/ProgressBar.h>
#include <vb/data/TriMatrix.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class Walker {
public:
    Walker() : pos(3, 0), Max(3, TriMatrix{INT_MIN}), Min(3, TriMatrix{INT_MAX}) {
        for (unsigned i = 0; i < 3; ++i) {
            Max[i].put({0, 0}, 0);
            Min[i].put({0, 0}, 0);
        }
    }

    auto step(unsigned dir, int add) -> int {
        if (add == 0) { return 0; }
        if (add > 0) {
            if (pos[dir] < Max[dir].at({pos[(dir + 1) % 3], pos[(dir + 2) % 3]})) return 0;
        }
        /* else */ {
            if (pos[dir] > Min[dir].at({pos[(dir + 1) % 3], pos[(dir + 2) % 3]})) return 0;
        }
        pos[dir] += add;
        reframe();
        ++length;
        return 1;
    }

    void reframe() {
        for (unsigned i = 0; i < 3; ++i) {
            if (pos[i] > Max[i].at({pos[(i + 1) % 3], pos[(i + 2) % 3]})) Max[i].put({pos[(i + 1) % 3], pos[(i + 2) % 3]}, pos[i]);
            if (pos[i] < Min[i].at({pos[(i + 1) % 3], pos[(i + 2) % 3]})) Min[i].put({pos[(i + 1) % 3], pos[(i + 2) % 3]}, pos[i]);
        }
    }

    [[nodiscard]] auto norm1() const -> int { return abs(pos[0]) + abs(pos[1]) + abs(pos[2]); }

    size_t                 length = 0;
    vector<int>            pos;
    vector<TriMatrix<int>> Max, Min;
};

auto operator<<(ostream &o, const Walker &W) -> ostream & {
    o << W.pos[0] << ' ' << W.pos[1] << ' ' << W.pos[2] << ' ' << W.norm1() << endl;
    return o;
}

auto main(int argc, char **argv) -> int {
    Hub const   H("3D prudent walker", argc, argv, "l=1000,n=1");
    size_t      l = H['l'], n = H['n'];
    vector<int> ends(l, 0);

    {
        ProgressBar PB(n * l);
        size_t      t = 0;
        for (size_t i = 0; i < n; ++i) {
            Walker W;
            while (W.length < l) {
                if (n == 1) cout << W;
                ends[W.length] += W.norm1();
                while (0 == W.step(prng.uniform_int(3U), 2 * (prng.uniform_int(2)) - 1)) {};
                PB.set(++t);
            }
        }
    }

    if (n > 1) {
        for (size_t i = 0; i < l; ++i) cout << i << ' ' << double(ends[i]) / double(n) << endl;
    }
}
