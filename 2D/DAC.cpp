#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

int main(int argc, char **argv) {
    Hub    H("Divide-and-color model", argc, argv, "n=300,p=.3,q=.4");
    int    n = H['n'];
    double p = H['p'], q = H['q'];

    // Connectivity (&1 -> to the right, &2 -> downwards)
    vector<uint8_t> connect(n * n);
    for (auto &c : connect) c = (prng.bernoulli(p) ? 1 : 0) + (prng.bernoulli(p) ? 2 : 0);

    spdlog::info("Computing connected components ...");

    vector<int64_t> cluster(n * n);
    for (int i = 0; i < n * n; ++i) cluster[i] = i;

    bool dirty = true;
    while (dirty) {
        dirty = false;
        for (int64_t x = 0; x < n; ++x) {
            for (int64_t y = 0; y < n; ++y) {
                if ((x > 0) && ((connect[(x - 1) + n * y] & 1u) != 0) && (cluster[x + n * y] > cluster[(x - 1) + n * y])) {
                    dirty              = true;
                    cluster[x + n * y] = cluster[(x - 1) + n * y];
                }
                if ((x < n - 1) && ((connect[x + n * y] & 1u) != 0) && (cluster[x + n * y] > cluster[(x + 1) + n * y])) {
                    dirty              = true;
                    cluster[x + n * y] = cluster[(x + 1) + n * y];
                }
                if ((y > 0) && ((connect[x + n * (y - 1)] & 2u) != 0) && (cluster[x + n * y] > cluster[x + n * (y - 1)])) {
                    dirty              = true;
                    cluster[x + n * y] = cluster[x + n * (y - 1)];
                }
                if ((y < n - 1) && ((connect[x + n * y] & 2u) != 0) && (cluster[x + n * y] > cluster[x + n * (y + 1)])) {
                    dirty              = true;
                    cluster[x + n * y] = cluster[x + n * (y + 1)];
                }
            }
        }
    }

    spdlog::info(" ... Done.");

    vector<uint8_t> color(n * n);
    for (auto &c : color) c = (prng.bernoulli(q) ? 255 : 0);

    Image img(H.title, {n, n});

    for (int64_t x = 0; x < n; ++x)
        for (int64_t y = 0; y < n; ++y) img.put({x, y}, Grey(color[cluster[x + n * y]]));
    img.show();
    img.pause();
}
