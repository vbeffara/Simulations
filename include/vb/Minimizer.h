#pragma once /// @file
#include <vb/math/LinearAlgebra.h>

namespace vb {
    template <typename T> class Minimizer {
    public:
        Minimizer(size_t n_);
        Minimizer(size_t n_, std::function<T(const Vector<T> &, Vector<T> *)> fg_);
        Minimizer(size_t n_, std::function<T(const Vector<T> &)> f_, std::function<Vector<T>(const Vector<T> &)> g_);

        T compute(const Vector<T> &x = Vector<T>(0));

        void line_search(const Vector<T> &d);

        T minimize_grad(const Vector<T> &x0);
        T minimize_bfgs(const Vector<T> &x0, const Vector<T> &W0 = Vector<T>(0));
        T minimize_fr(const Vector<T> &x0);
        T minimize_pr(const Vector<T> &x0);
        T minimize_qn(const Vector<T> &x0);

        size_t n = 0;

        std::function<T(const Vector<T> &)>              f;
        std::function<Vector<T>(const Vector<T> &)>      g;
        std::function<T(const Vector<T> &, Vector<T> *)> fg;
        std::function<void(const Vector<T> &, T)>        cb;

        Vector<T> x;
        T         fx;
        Vector<T> gx;
        Vector<T> old_x;
        T         old_fx;
        Vector<T> old_gx;

        T   er;
        int ler = 0;
    };

#ifdef UNIT_TESTS
    TEST_CASE("vb::Minimizer") {
        auto f = [](const Vector<double> &x, void *) {
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
#endif
} // namespace vb
