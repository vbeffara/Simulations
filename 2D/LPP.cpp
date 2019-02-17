#include <vb/Bitmap.h>
#include <vb/util/PRNG.h>

using namespace std;
using namespace vb;

int main(int argc, char **argv) {
    Hub    H("Last-passage percolation", argc, argv, "n=500,r=.1,l=.01,d=0");
    int    n = H['n'];
    double r = H['r'], l = H['l'], d = H['d'];

    Image img(H.title, {n, n});
    img.show();

    vector<int> field(n, 0);

    for (int x = 0; x < n; ++x) {
        if (prng.bernoulli(r)) {
            img.put({x, 0}, WHITE);
            field[x] = 1;
        }
    }

    img.update();

    for (int y = 1; y < n; ++y) {
        for (int x = 0; x < n; ++x)
            if ((prng.bernoulli(l)) || ((x == y) && (prng.bernoulli(d)))) {
                img.put({x, y}, WHITE);
                field[x]++;
                int xx = (x + 1) % n;
                while (field[xx] == 0) {
                    img.put({xx, y}, WHITE);
                    xx = (xx + 1) % n;
                }
                field[xx]--;
                img.put({xx, y}, WHITE);
            }
        for (int x = 0; x < n; ++x)
            if (field[x] != 0) img.put({x, y}, WHITE);

        if (y % 100 == 0) img.update();
    }
}
