#pragma once
#include <vb/Polynomial.h>
#include <vb/mp.h>
#include <fplll.h>
#include <optional>

namespace vb {
    std::optional<cln::cl_UP_R> guess1(const cln::cl_R & x, int nd);
    std::optional<cln::cl_UP_N> guess_r(const cln::cl_N & x, int nd);

    template <typename T> std::optional<Polynomial<mpz_int>> guess(const T & x, int nd) {
        mpz_int m = pow(mpz_int(10), nd * 2 / 3);

        for (int d = 1; d <= nd / 10; ++d) {
            T             t{1};
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

            Polynomial<mpz_int> P(begin(V), end(V));
            if (V[d] < 0) P = -P;
            auto PP = derivative(P);

            T xx = x, ox = x + 1, er = 2;
            while (real(abs(xx - ox)) < real(er)) {
                er = abs(xx - ox);
                ox = xx;
                xx -= eval(P, xx) / eval(PP, xx);
            }
            if (abs(xx - x) < pow(real_t(10), 5 - nd)) return P;
        }

        return {};
    }

#ifdef UNIT_TESTS
    TEST_CASE("NumberTheory guess functions") {
        cln::default_float_format = cln::float_format(100);
        cln::cl_F z("0.9162918442410306144165008200767499077603397502333144975769802641182380808885019256331544308341889255");
        CHECK(str(*(guess1(z, 100))) == "1*z^5 + -3*z^4 + 12*z^3 + -2*z^2 + 1*z + -7");

        cln::cl_F z1("0.1722882583776278670500267959231284336682007863854856624427574750255049273322927690638923632");
        cln::cl_F z2("0.5302487364574217190358808797265653491226567421626168710631761419479819886565504921987031543");
        cln::cl_N zc = cln::complex(z1, z2);
        CHECK(str(*(guess_r(zc, 100))) == "1*z^8 + 6*z^7 + 17*z^6 + 18*z^5 + 25*z^4 + -18*z^3 + 17*z^2 + -6*z + 1");

        CHECK(!guess1(cln::pi(z), 100));
    }
#endif
} // namespace vb
