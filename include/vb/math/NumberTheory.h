#pragma once
#include <fplll.h>
#include <optional>
#include <vb/math/Polynomial.h>
#include <vb/util/mp.h>

using fplll::vector;
using fplll::Z_NR;
using fplll::ZZ_mat;

namespace vb {
    template <typename T> auto guess(const T &x, unsigned nd) -> std::optional<Polynomial<mpz_int>> {
        real_t m{pow(real_t{10, x.precision()}, nd * 2 / 3)};
        for (unsigned d = 1; d <= nd / 10; ++d) {
            T             t{1, x.precision()};
            ZZ_mat<mpz_t> M(int(d + 1), int(d + 3));

            for (unsigned i = 0; i <= d; ++i) {
                M[int(i)][0]          = mpz_int(real(t) * m).backend().data();
                M[int(i)][1]          = mpz_int(imag(t) * m).backend().data();
                M[int(i)][int(i + 2)] = 1;
                t *= x;
            }

            lll_reduction(M);
            std::vector<Z_NR<mpz_t>> o;
            shortest_vector(M, o);

            vector<mpz_int> V(d + 1, 0);
            for (unsigned j = 0; j < d + 1; ++j) {
                mpz_int const ai = o[j].get_data();
                for (unsigned i = 0; i <= d; ++i) V[i] += ai * M[int(j)][int(i + 2)].get_data();
            }

            Polynomial<mpz_int> P(V);
            if (P.degree() == 0) continue;
            if (V[d] < 0) P *= -1;
            auto PP = P.derivative();

            T xx = x, ox = x + 1, er = 2;
            while (real(abs(xx - ox)) < real(er)) {
                er = abs(xx - ox);
                if (real(er) < pow(real_t{10, x.precision()}, -5 * int(nd))) er = 0;
                ox = xx;
                xx -= P(xx) / PP(xx);
            }
            if (abs(xx - x) < pow(real_t{10, x.precision()}, 5 - int(nd))) return P;
        }

        return {};
    }
} // namespace vb
