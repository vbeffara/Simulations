#include <map>
#include <vb/Coloring.h>
#include <vb/math/math.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

template <typename T> auto sm3(size_t i, size_t j, size_t k) -> T {
    static T np = 9 * pi_<T>();
    if (i < j) return sm3<T>(j, i, k);
    if (i < k) return sm3<T>(k, i, j);
    if (i == 0) return 1.0;
    return sqrt(T(i + j + k) / (np * T(i))) * sm3<T>(i - 1, j, k);
}

template <typename T> class RPoly {
public:
    RPoly(size_t n_, bool p_, const string &gen) : n(n_), p(p_), A(n + 1) {
        map<string, function<double()>> generators;
        generators.emplace("gaussian", [] { return prng.gaussian(0, 1); });
        generators.emplace("bernoulli", [] { return prng.bernoulli(.5) ? 1 : -1; });

        for (size_t i = 0; i <= n; ++i)
            for (size_t j = 0; j <= n - i; ++j) {
                T a(generators[gen]());
                T b = a * sm3<T>(i, j, n - i - j);
                A[i].push_back(b);
            }
    }

    auto operator()(T x, T y) -> T {
        if (p) {
            T r = (x * x + y * y);
            if (r >= 1) return 0;
            x /= 1 - r;
            y /= 1 - r;
        }
        T out = A[n][0];
        for (size_t i = n - 1; i <= n; --i) {
            T pi = A[i][n - i];
            for (size_t j = n - i - 1; j <= n; --j) pi = y * pi + A[i][j];
            out = x * out + pi;
        }
        return out;
    }

    auto operator()(cpx z) -> Color {
        T val = (*this)(T(real(z)), T(imag(z)));
        if (val == T(0)) return NOCOLOR;
        return HSV(val > 0 ? 0 : .5, .8, .8);
    }

    size_t            n;
    bool              p;
    vector<vector<T>> A;
};

auto main(int argc, char **argv) -> int {
    CLP clp(argc, argv, "Random polynomial in 2 variables");
    auto n   = clp.param("n", 100, "Polynomial degree");
    auto gen = clp.param("g", "gaussian"s, "Coefficient generator");
    auto s   = clp.param("s", size_t(0), "Random seed (0 for default)");
    auto p   = clp.flag("p", "Project to disk");
    clp.finalize();

    if (s > 0) prng.seed(s);
    RPoly<double> const P(size_t(n), p, gen);
    double const        l = p ? 1 : 10;
    Coloring            C(clp.title, cpx(-l, -l), cpx(l, l), 800, P);
    C.show();
    C.output(clp.title);
    Fl::run();
}
