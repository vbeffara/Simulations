#include <vb/Bitmap.h>
#include <vb/ProgressBar.h>
#include <vb/util/Hub.h>

using namespace vb;

const Color            CC[] = {RED, GREEN, BLUE, YELLOW};
gsl::span<const Color> C{CC};

int main(int argc, char **argv) {
    Hub    H("Rotor-Router Model", argc, argv, "n=500");
    size_t n = H['n'];

    Image img(H.title, {n, n});
    img.show();

    ucoo z{n / 2, n / 2};
    while (img.contains(z)) {
        Color c = img.at(z);
        if (c == BLACK) {
            img.put(z, C[0]);
            z = {n / 2, n / 2};
        } else
            for (int i = 0; i < 4; ++i)
                if (c == C[i]) {
                    img.put(z, C[(i + 1) % 4]);
                    z = ucoo(coo(z) + dz[i]);
                }
    }

    img.output_png(H.title);
}
