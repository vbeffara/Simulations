#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;

auto main(int argc, char **argv) -> int {
    Hub    H("Gradient percolation", argc, argv, "n=500,p=0,q=1,W=0,H=0");
    size_t n  = H['n'];
    size_t w  = H['W'];
    size_t h  = H['H'];
    double p1 = H['p'];
    double p2 = H['q'];

    if ((w * h == 0) && (w + h != 0)) {
        std::cerr << "Please specify both width and height, or neither." << std::endl;
        exit(1);
    }

    if (w * h == 0) {
        w = n;
        h = n;
    }

    Image img(H.title, {w, h});

    for (size_t x = 0; x < w; ++x) {
        for (size_t y = 0; y < h; ++y) {
            if (prng.bernoulli(p1 + (p2 - p1) * y / h))
                img.put({x, y}, Grey(200));
            else
                img.put({x, y}, Grey(100));
        }
    }

    for (size_t i = 0; i < w; ++i) img.put({i, h - 1}, Grey(200));

    img.show();
    img.fill({0, h - 1}, Color(255, 0, 0));
    img.update();
    img.output(H.title);
    return 0;
}
