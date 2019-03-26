#include <vb/CoarseImage.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

int main(int argc, char **argv) {
    Hub          H("Randomized Polynuclear Growth", argc, argv, "n=500,e=0.01,f,c=0");
    const size_t n = H['n'];
    const double e = H['e'];
    const bool   f = H['f'];
    const double c = H['c'];

    CoarseImage img(H.title, {n, n}, 10);
    img.show();

    vector<size_t> height(n);
    for (size_t i = 0; i < n; ++i) {
        height[i] = 0;
        img.put(coo(ucoo({i, 0})), true);
    }

    bool running = true;
    while (running) {
        auto x = 1 + prng.uniform_int(n - 2);
        auto h = height[x];
        auto d = prng.uniform_int(1U << 14U);

        if (d != 0) { // If trying to grow
            if ((height[x - 1] > h) || (height[x + 1] > h) || (prng.bernoulli(e)) || ((c > 0) && (x == n / 2) && (prng.bernoulli(c))))
                ++height[x];
        } else { // If trying to shrink
            if ((height[x - 1] < h) || (height[x + 1] < h)) --height[x];
        }

        if (height[x] >= n) break;

        if (height[x] < h) img.put(coo(ucoo{x, h}), false);
        if (height[x] > h) img.put(coo(ucoo{x, h + 1}), true);

        if (f) {
            height[0] = height[1];
            img.put(coo(ucoo{0, height[0]}), true);
            img.put(coo(ucoo{0, height[0] + 1}), false);

            height[n - 1] = height[n - 2];
            img.put(coo(ucoo{n - 1, height[n - 1]}), true);
            img.put(coo(ucoo{n - 1, height[n - 1] + 1}), false);
        }
    }
}
