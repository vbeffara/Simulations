#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

auto main(int argc, char **argv) -> int {
    Hub const    H("Divide-and-color model", argc, argv, "n=300,p=.3,q=.4");
    size_t const n = H['n'];
    double p = H['p'], q = H['q'];

    // Connectivity (&1 -> to the right, &2 -> downwards)
    Array<uint8_t> connect({n, n});
    for (auto z : coo_range(connect.size)) connect[z] = uint8_t((prng.bernoulli(p) ? 1 : 0) + (prng.bernoulli(p) ? 2 : 0));

    Array<size_t> cluster({n, n});
    for (auto z : coo_range(cluster.size)) cluster[z] = z.x + n * z.y;

    bool dirty = true;
    while (dirty) {
        dirty = false;
        for (auto z : coo_range(cluster.size)) {
            if ((z.x < n - 1) && ((connect[z] & 1U) != 0) && (cluster[z] > cluster[z + dz[0]])) {
                dirty      = true;
                cluster[z] = cluster[z + dz[0]];
            }
            if ((z.y < n - 1) && ((connect[z] & 2U) != 0) && (cluster[z] > cluster[z + dz[1]])) {
                dirty      = true;
                cluster[z] = cluster[z + dz[1]];
            }
            if ((z.x > 0) && ((connect[z + dz[2]] & 1U) != 0) && (cluster[z] > cluster[z + dz[2]])) {
                dirty      = true;
                cluster[z] = cluster[z + dz[2]];
            }
            if ((z.y > 0) && ((connect[z + dz[3]] & 2U) != 0) && (cluster[z] > cluster[z + dz[3]])) {
                dirty      = true;
                cluster[z] = cluster[z + dz[3]];
            }
        }
    }

    vector<uint8_t> color(size_t(n * n));
    for (auto &c : color) c = (prng.bernoulli(q) ? 255 : 0);

    Image img(H.title, {n, n});
    for (auto z : coo_range(img.size)) img.put(z, Grey(color[cluster[z]]));
    img.show();
    img.pause();
}
