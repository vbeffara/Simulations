#include <vb/NumberTheory.h>
#include <fplll.h>

namespace vb {
    using namespace cln;

    std::optional<Polynomial<mpz_int>> guess(const mpf_float & x, int nd) {
        if (x == 0) return {{0, 1}};

        mpz_int m = pow(mpz_int(10), nd * 2 / 3);

        for (int d = 1; d <= nd / 10; ++d) {
            mpf_float     t = x / x;
            ZZ_mat<mpz_t> M(d + 1, d + 2);

            for (int i = 0; i <= d; ++i) {
                M[i][0]     = mpz_int(t * m).backend().data();
                M[i][i + 1] = 1;
                t *= x;
            }

            lll_reduction(M);
            vector<Z_NR<mpz_t>> o;
            shortest_vector(M, o);

            vector<mpz_int> V(d + 1, 0);
            for (int j = 0; j < d + 1; ++j) {
                mpz_int ai = o[j].get_data();
                for (int i = 0; i <= d; ++i) V[i] += ai * M[j][i + 1].get_data();
            }

            Polynomial<mpz_int> P(begin(V), end(V));
            if (V[d] < 0) P = -P;
            auto PP = derivative(P);

            mpf_float xx = x, ox = x + 1, er = 2;
            while (abs(xx - ox) < er) {
                er = abs(xx - ox);
                ox = xx;
                xx -= eval(P, xx) / eval(PP, xx);
            }
            if (abs(xx - x) < pow(mpf_float(10), 5 - nd)) return P;
        }

        return {};
    }

    std::optional<Polynomial<mpz_int>> guess(const mpfr_float & x, int nd) {
        if (x == 0) return {{0, 1}};

        mpz_int m = pow(mpz_int(10), nd * 2 / 3);

        for (int d = 1; d <= nd / 10; ++d) {
            mpfr_float    t = x / x;
            ZZ_mat<mpz_t> M(d + 1, d + 2);

            for (int i = 0; i <= d; ++i) {
                M[i][0]     = mpz_int(t * m).backend().data();
                M[i][i + 1] = 1;
                t *= x;
            }

            lll_reduction(M);
            vector<Z_NR<mpz_t>> o;
            shortest_vector(M, o);

            vector<mpz_int> V(d + 1, 0);
            for (int j = 0; j < d + 1; ++j) {
                mpz_int ai = o[j].get_data();
                for (int i = 0; i <= d; ++i) V[i] += ai * M[j][i + 1].get_data();
            }

            Polynomial<mpz_int> P(begin(V), end(V));
            if (V[d] < 0) P = -P;
            auto PP = derivative(P);

            mpfr_float xx = x, ox = x + 1, er = 2;
            while (abs(xx - ox) < er) {
                er = abs(xx - ox);
                ox = xx;
                xx -= eval(P, xx) / eval(PP, xx);
            }
            if (abs(xx - x) < pow(mpfr_float(10), 5 - nd)) return P;
        }

        return {};
    }

    std::optional<Polynomial<mpz_int>> guess(const mpc_complex & x, int nd) {
        if (x == 0) return {{0, 1}};

        mpz_int m = pow(mpz_int(10), nd * 2 / 3);

        for (int d = 1; d <= nd / 5; ++d) {
            mpc_complex   t{1, 0};
            ZZ_mat<mpz_t> M(d + 1, d + 3);

            for (int i = 0; i <= d; ++i) {
                M[i][0]     = mpz_int(t.real() * m).backend().data();
                M[i][1]     = mpz_int(imag(t) * m).backend().data();
                M[i][i + 2] = 1;
                t *= x;
            }

            lll_reduction(M);
            vector<Z_NR<mpz_t>> o;
            shortest_vector(M, o);

            vector<mpz_int> V(d + 1, 0);
            for (int j = 0; j < d + 1; ++j) {
                mpz_int ai = o[j].get_data();
                for (int i = 0; i <= d; ++i) V[i] += ai * M[j][i + 2].get_data();
            }

            Polynomial<mpz_int> P(begin(V), end(V));
            if (V[d] < 0) P = -P;
            auto PP = derivative(P);

            mpc_complex xx = x, ox = x + 1;
            mpfr_float  er = 2;
            while (abs(xx - ox) < er) {
                er = abs(xx - ox);
                ox = xx;
                xx -= eval(P, xx) / eval(PP, xx);
            }
            if (abs(xx - x) < pow(mpf_float(10), 5 - nd)) return P;
        }

        return {};
    }

    std::optional<cl_UP_R> guess(const cl_R & x, int nd) {
        cl_F xf = cl_float(x);
        auto m  = expt(cl_float(10, xf), nd * 2 / 3);

        for (int d = 1; d <= nd / 10; ++d) {
            auto          t = cl_float(1, xf);
            ZZ_mat<mpz_t> M(d + 1, d + 2);

            for (int i = 0; i <= d; ++i) {
                M[i][0].set_str(fmt::format("{}", round1(t * m)).c_str());
                M[i][i + 1] = 1;
                t *= xf;
            }

            lll_reduction(M);
            vector<Z_NR<mpz_t>> o;
            shortest_vector(M, o);

            vector<cl_I> V(d + 1, 0);
            for (int j = 0; j < d + 1; ++j) {
                cl_I ai = fmt::format("{}", o[j]).c_str();
                if (ai != 0)
                    for (int i = 0; i <= d; ++i) V[i] += ai * cl_I{fmt::format("{}", M[j][i + 1]).c_str()};
            }

            auto P = find_univpoly_ring(cl_R_ring, cl_symbol("z"))->create(d);
            for (int i = 0; i <= d; ++i) set_coeff(P, i, V[i]);
            finalize(P);
            if (V[d] < 0) P = -P;

            auto PP = deriv(P);
            cl_F xx = xf, ox = xf + 1;
            while (abs(xx - ox) > expt(cl_float(10), 5 - nd)) {
                ox = xx;
                xx -= P(xx) / PP(xx);
            }
            if (abs(xx - xf) < expt(cl_float(10, xf), 5 - nd)) return P;
        }

        return std::nullopt;
    }

    std::optional<cl_UP_N> guess_r(const cl_N & x, int nd) {
        auto m = expt(cl_float(10), nd * 2 / 3);

        for (int d = 1; d <= nd / 10; ++d) {
            cl_N          t = cl_float(1, the<cl_F>(realpart(x)));
            ZZ_mat<mpz_t> M(d + 1, d + 3);

            for (int i = 0; i <= d; ++i) {
                auto re = fmt::format("{}", round1(realpart(t) * m));
                auto im = fmt::format("{}", round1(imagpart(t) * m));
                M[i][0].set_str(re.c_str());
                M[i][1].set_str(im.c_str());
                M[i][i + 2] = 1;
                t *= x;
            }

            lll_reduction(M);
            vector<Z_NR<mpz_t>> o;
            shortest_vector(M, o);

            vector<cl_I> V(2 * (d + 1), 0);
            for (int j = 0; j < d + 1; ++j) {
                cl_I ai = fmt::format("{}", o[j]).c_str();
                if (ai != 0)
                    for (int i = 0; i < d + 1; ++i) V[i] += ai * cl_I{fmt::format("{}", M[j][i + 2]).c_str()};
            }

            auto P = find_univpoly_ring(cl_C_ring, cl_symbol("z"))->create(d);
            for (int i = 0; i <= d; ++i) set_coeff(P, i, V[i]);
            finalize(P);
            if (realpart(coeff(P, d)) < 0) P = -P;
            if (coeff(P, d) == 0) continue;

            auto PP = deriv(P);
            cl_N xx = x, ox = x + 1;
            while (abs(xx - ox) > expt(cl_float(10), 5 - nd)) {
                ox = xx;
                xx -= P(xx) / PP(xx);
            }
            if (abs(xx - x) < expt(cl_float(10), 10 - nd)) return P;
        }

        return std::nullopt;
    }
} // namespace vb
