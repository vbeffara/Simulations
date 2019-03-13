#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace std;
using namespace vb;

int main(int argc, char **argv) {
    Hub    H("Last-passage percolation", argc, argv, "n=500,r=.1,l=.01,d=0");
    size_t n = H['n'];
    double r = H['r'], l = H['l'], d = H['d'];

    // TODO: unsigned coo somewhere
    Image img(H.title, {int(n), int(n)});
    img.show();

    vector<int> field(n, 0);

    for (size_t x = 0; x < n; ++x) {
        if (prng.bernoulli(r)) {
            img.put({int(x), 0}, WHITE);
            field[x] = 1;
        }
    }

    img.update();

    for (size_t y = 1; y < n; ++y) {
        for (size_t x = 0; x < n; ++x)
            if ((prng.bernoulli(l)) || ((x == y) && (prng.bernoulli(d)))) {
                img.put({int(x), int(y)}, WHITE);
                field[x]++;
                auto xx = (x + 1) % n;
                while (field[xx] == 0) {
                    img.put({int(xx), int(y)}, WHITE);
                    xx = (xx + 1) % n;
                }
                field[xx]--;
                img.put({int(xx), int(y)}, WHITE);
            }
        for (unsigned x = 0; x < n; ++x)
            if (field[x] != 0) img.put({int(x), int(y)}, WHITE);

        if (y % 100 == 0) img.update();
    }
}
