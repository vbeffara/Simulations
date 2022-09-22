#include <vb/Coloring.h>
#include <vb/util/Hub.h>

using namespace vb;

int t;

auto M(cpx c) -> Color {
    cpx z = c;
    for (int i = 0; i < t; ++i) {
        if (real(z) * real(z) + imag(z) * imag(z) > 4) return Indexed(i);
        z = z * z + c;
    }
    return BLACK;
}

auto main(int argc, char **argv) -> int {
    Hub const H("Mandelbrot set", argc, argv, "n=800,t=1000");
    t = H['t'];
    Coloring C(H.title, cpx(-2.2, -1.5), cpx(1.2, 1.5), H['n'], M);
    C.show();
    return Fl::run();
}
