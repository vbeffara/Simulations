#include <vb/Coloring.h>
#include <vb/util/CLP.h>

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
    CLP clp(argc, argv, "Mandelbrot set");
    auto n = clp.param("n", 800, "Image size");
    t = clp.param("t", 1000, "Max iterations");
    clp.finalize();
    Coloring C(clp.title, cpx(-2.2, -1.5), cpx(1.2, 1.5), n, M);
    C.show();
    return Fl::run();
}
