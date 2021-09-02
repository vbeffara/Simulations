#include <vb/Coloring.h>
#include <vb/math/math.h>
#include <vb/util/Hub.h>
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
    RPoly(const Hub &H, size_t n_, bool(p_)) : n(n_), p(p_), A(n + 1) {
        map<string, function<double()>> generators;
        generators.emplace("gaussian", [] { return prng.gaussian(0, 1); });
        generators.emplace("bernoulli", [] { return prng.bernoulli(.5) ? 1 : -1; });

        for (size_t i = 0; i <= n; ++i)
            for (size_t j = 0; j <= n - i; ++j) {
                T a(generators[H['g']]());
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
    Hub H("Random polynomial in 2 variables", argc, argv, "n=100,g=gaussian,s=0,p");
    if (unsigned s = H['s']; s > 0) prng.seed(s);
    RPoly<double> P(H, H['n'], H['p']);
    double        l = H['p'] ? 1 : 10;
    Coloring      C(H.title, cpx(-l, -l), cpx(l, l), 800, P);
    C.show();
    C.output(H.title);
    Fl::run();
}
