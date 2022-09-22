#include <vb/Bitmap.h>
#include <vb/ProgressBar.h>
#include <vb/util/Hub.h>

using namespace vb;

auto main(int argc, char **argv) -> int {
    const std::vector<Color> C{RED, GREEN, BLUE, YELLOW};

    Hub const    H("Rotor-Router Model", argc, argv, "n=500");
    size_t const n = H['n'];

    Image img(H.title, {n, n});
    img.show();

    ucoo z{n / 2, n / 2};
    while (img.fits(coo(z), 1)) {
        Color const c = img.at(z);
        if (c == BLACK) {
            img.put(z, C[0]);
            z = {n / 2, n / 2};
        } else
            for (unsigned i = 0; i < 4; ++i)
                if (c == C[i]) {
                    img.put(z, C[(i + 1) % 4]);
                    z += dz[i];
                }
    }

    img.output_png(H.title);
}
