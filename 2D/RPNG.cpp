#include <vb/CoarseImage.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

auto main(int argc, char **argv) -> int {
    CLP clp(argc, argv, "Randomized Polynuclear Growth");
    size_t n = clp.param("n", size_t(500), "Grid size");
    auto   e = clp.param("e", 0.01, "Spontaneous growth rate");
    auto   f = clp.flag("f", "Fixed boundary");
    auto   c = clp.param("c", 0.0, "Center growth rate");
    clp.finalize();

    CoarseImage img(clp.title, {n, n}, 10);
    img.show();

    vector<size_t> height(n);
    for (size_t i = 0; i < n; ++i) {
        height[i] = 0;
        img.put({i, 0}, true);
    }

    bool const running = true;
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

        if (height[x] < h) img.put({x, h}, false);
        if (height[x] > h) img.put({x, h + 1}, true);

        if (f) {
            height[0] = height[1];
            img.put({0, height[0]}, true);
            img.put({0, height[0] + 1}, false);

            height[n - 1] = height[n - 2];
            img.put({n - 1, height[n - 1]}, true);
            img.put({n - 1, height[n - 1] + 1}, false);
        }
    }
}
