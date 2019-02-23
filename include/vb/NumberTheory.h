#pragma once
#include <fplll.h>
#include <optional>
#include <vb/Polynomial.h>

namespace vb {
    template <typename T> std::optional<Polynomial<mpz_int>> guess(const T &x, int nd) {
        real_t m{pow(real_t{10, x.precision()}, nd * 2 / 3)};
        for (int d = 1; d <= nd / 10; ++d) {
            T             t{1, x.precision()};
            ZZ_mat<mpz_t> M(d + 1, d + 3);

            for (int i = 0; i <= d; ++i) {
                M[i][0]     = mpz_int(real(t) * m).backend().data();
                M[i][1]     = mpz_int(imag(t) * m).backend().data();
                M[i][i + 2] = 1;
                t *= x;
            }

            lll_reduction(M);
            std::vector<Z_NR<mpz_t>> o;
            shortest_vector(M, o);

            vector<mpz_int> V(d + 1, 0);
            for (int j = 0; j < d + 1; ++j) {
                mpz_int ai = o[j].get_data();
                for (int i = 0; i <= d; ++i) V[i] += ai * M[j][i + 2].get_data();
            }

            // TODO: fix construction
            Polynomial<mpz_int> P(boost::math::tools::polynomial<mpz_int>(begin(V), end(V)));
            if (P.degree() == 0) continue;
            if (V[d] < 0) P.P = -P.P;
            auto PP = P.derivative();

            T xx = x, ox = x + 1, er = 2;
            while (real(abs(xx - ox)) < real(er)) {
                er = abs(xx - ox);
                if (real(er) < pow(real_t{10, x.precision()}, -5 * nd)) er = 0;
                ox = xx;
                xx -= P(xx) / PP(xx);
            }
            if (abs(xx - x) < pow(real_t{10, x.precision()}, 5 - nd)) return P;
        }

        return {};
    }

#ifdef UNIT_TESTS
    TEST_CASE("NumberTheory guess functions") {}
#endif
} // namespace vb
