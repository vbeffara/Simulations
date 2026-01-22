#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;

auto main(int argc, char **argv) -> int {
    CLP clp(argc, argv, "Gradient percolation");
    auto n = clp.param("n", size_t(500), "Simulation size (used if W,H unset)");
    auto p1 = clp.param("p", 0.0, "Bottom probability");
    auto p2 = clp.param("q", 1.0, "Top probability");
    auto W = clp.param("W", size_t(0), "Width (0 -> use n)");
    auto Hh = clp.param("H", size_t(0), "Height (0 -> use n)");
    clp.finalize();

    size_t w = W, h = Hh;
    if ((w * h == 0) && (w + h != 0)) {
        std::cerr << "Please specify both width and height, or neither." << std::endl;
        return 1;
    }
    if (w * h == 0) { w = n; h = n; }

    Image img(clp.title, {w, h});

    for (size_t x = 0; x < w; ++x) {
        for (size_t y = 0; y < h; ++y) {
            if (prng.bernoulli(p1 + (p2 - p1) * double(y) / double(h)))
                img.put({x, y}, Grey(200));
            else
                img.put({x, y}, Grey(100));
        }
    }

    for (size_t i = 0; i < w; ++i) img.put({i, h - 1}, Grey(200));

    img.show();
    img.fill({0, h - 1}, Color(255, 0, 0));
    img.update();
    img.output(clp.title);
    return 0;
}
