#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace std;
using namespace vb;

auto main(int argc, char **argv) -> int {
    Hub const    H("Last-passage percolation", argc, argv, "n=500,r=.1,l=.01,d=0");
    size_t const n = H['n'];
    double r = H['r'], l = H['l'], d = H['d'];

    Image img(H.title, {n, n});
    img.show();

    vector<int> field(n, 0);

    for (size_t x = 0; x < n; ++x) {
        if (prng.bernoulli(r)) {
            img.put({x, 0}, WHITE);
            field[x] = 1;
        }
    }

    img.update();

    for (size_t y = 1; y < n; ++y) {
        for (size_t x = 0; x < n; ++x)
            if ((prng.bernoulli(l)) || ((x == y) && (prng.bernoulli(d)))) {
                img.put({x, y}, WHITE);
                field[x]++;
                auto xx = (x + 1) % n;
                while (field[xx] == 0) {
                    img.put({xx, y}, WHITE);
                    xx = (xx + 1) % n;
                }
                field[xx]--;
                img.put({xx, y}, WHITE);
            }
        for (size_t x = 0; x < n; ++x)
            if (field[x] != 0) img.put({x, y}, WHITE);

        if (y % 100 == 0) img.update();
    }
}
