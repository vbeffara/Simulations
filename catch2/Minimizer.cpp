#include "catch2/catch.hpp"
#include <vb/Minimizer.h>

using namespace vb;

TEST_CASE("vb::Minimizer") {
    auto f = [](const Vector<double> &x, void * /*unused*/) {
        double o = 0;
        for (int i = 0; i < 400; ++i) o += (1 - cos(x[i] / (i + 1)));
        return o;
    };

    auto g = [](const Vector<double> &x) {
        Vector<double> out(400);
        for (int i = 0; i < 400; ++i) out[i] = sin(x[i] / (i + 1)) / (i + 1);
        return out;
    };

    auto fg = [](const Vector<double> &x, Vector<double> *g) {
        double o = 0;
        for (int i = 0; i < 400; ++i) {
            o += (1 - cos(x[i] / (i + 1)));
            (*g)[i] = sin(x[i] / (i + 1)) / (i + 1);
        }
        return o;
    };

    Vector<double> x0(400);
    for (int i = 0; i < 400; ++i) x0[i] = cos(double(i));
    Vector<double> W0(400);
    for (int i = 0; i < 400; ++i) W0[i] = (i + 1) * (i + 1);

    Minimizer<double> M(400, fg);
    CHECK(M.minimize_qn(x0) < 1e-7);
    (void)f;
    (void)g;
}
