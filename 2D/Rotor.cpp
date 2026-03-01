#include <vb/Bitmap.h>
#include <vb/ProgressBar.h>
#include <vb/util/CLP.h>

using namespace vb;

auto main(int argc, char **argv) -> int {
    const std::vector<Color> C{RED, GREEN, BLUE, YELLOW};

    CLP clp(argc, argv, "Rotor-Router Model");
    auto n = clp.param("n", 500, "Grid size");
    clp.finalize();

    Image img(clp.title, {size_t(n), size_t(n)});
    img.show();

    ucoo z{size_t(n) / 2, size_t(n) / 2};
    while (img.fits(coo(z), 1)) {
        Color const c = img.at(z);
        if (c == BLACK) {
            img.put(z, C[0]);
            z = {size_t(n) / 2, size_t(n) / 2};
        } else
            for (unsigned i = 0; i < 4; ++i)
                if (c == C[i]) {
                    img.put(z, C[(i + 1) % 4]);
                    z += dz[i];
                }
    }

    img.output_png(clp.title);
}
