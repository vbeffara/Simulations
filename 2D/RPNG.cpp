#include <vb/CoarseImage.h>

using namespace vb;
using namespace std;

int main(int argc, char **argv) {
    Hub          H("Randomized Polynuclear Growth", argc, argv, "n=500,e=0.01,f,c=0");
    const int    n = H['n'];
    const double e = H['e'];
    const bool   f = H['f'];
    const double c = H['c'];

    CoarseImage img(H, n, n, 10);
    img.show();

    vector<int> height(n);
    for (int i = 0; i < n; ++i) {
        height[i] = 0;
        img.put({i, 0}, true);
    }

    bool running = true;
    while (running) {
        int64_t x = 1 + prng.uniform_int(n - 2);
        int     h = height[x];
        int     d = prng.uniform_int(1u << 14u);

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
