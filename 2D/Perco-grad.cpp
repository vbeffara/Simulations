#include <vb/Bitmap.h>

using namespace vb;

int main(int argc, char **argv) {
    Hub    H("Gradient percolation", argc, argv, "n=500,p=0,q=1,W=0,H=0");
    int    n  = H['n'];
    int    w  = H['W'];
    int    h  = H['H'];
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

    Image img(H, w, h);

    for (int x = 0; x < w; ++x) {
        for (int y = 0; y < h; ++y) {
            if (prng.bernoulli(p1 + (p2 - p1) * y / h))
                img.put({x, y}, Grey(200));
            else
                img.put({x, y}, Grey(100));
        }
    }

    for (int i = 0; i < w; ++i) img.put({i, h - 1}, Grey(200));

    img.show();
    img.fill({0, h - 1}, Color(255, 0, 0));
    img.update();
    img.output(H);
    return 0;
}
