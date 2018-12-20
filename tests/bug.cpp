#include <vb/Polynomial.h>
#include <vb/mp.h>
#include <fplll.h>
#include <optional>

namespace vb {
    template <typename T> std::optional<Polynomial<mpz_int>> guess(const T &x, int nd) {
        std::cerr << x.precision() << "\n";
        mpz_int m = pow(mpz_int(10), nd * 2 / 3);

        for (int d = 1; d <= nd / 10; ++d) {
            T t{1};
            std::cerr << t.precision() << "\n";
            ZZ_mat<mpz_t> M(d + 1, d + 3);

            for (int i = 0; i <= d; ++i) {
                M[i][0]     = mpz_int(real(t) * m).backend().data();
                M[i][1]     = mpz_int(imag(t) * m).backend().data();
                M[i][i + 2] = 1;
                t *= x;
                std::cerr << t.precision() << "\n";
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
            if (P.degree() == 0) continue;
            if (V[d] < 0) P = -P;
            auto PP = derivative(P);

            T xx = x, ox = x + 1, er = 2;
            while (real(abs(xx - ox)) < real(er)) {
                er = abs(xx - ox);
                if (real(er) < pow(real_t(10), -5 * nd)) er = 0;
                ox = xx;
                xx -= eval(P, xx) / eval(PP, xx);
            }
            if (abs(xx - x) < pow(real_t(10), 5 - nd)) return P;
        }

        return {};
    }

#ifdef UNIT_TESTS
    TEST_CASE("NumberTheory guess functions") {}
#endif
} // namespace vb

using namespace vb;
using namespace std;

int main() {
    real_t::default_precision(100);
    complex_t::default_precision(100);

    vector<string> xs;
    xs.emplace_back("0.9162918442410306144165008200767499077603397502333144975769802641182380808885019256331544308341889255");
    xs.emplace_back("1.36602540378443864676372317075293618347140262690519031402790348972596650845440001854057309338");
    xs.emplace_back("0.5877852522924731291687059546390727685976524376431459910722724807572784741623519575085040499");
    xs.emplace_back("0.8090169943749474241022934171828190588601545899028814310677243113526302314094512248536036021");
    xs.emplace_back("798303.3673469387755102040816326530612244897959183673469387755102040816326530612244897959183673469");
    xs.emplace_back("1.12266701157648291040117446489401135452719640061624479423083713099312644668872851836902659616");
    xs.emplace_back("0.63827397417446081629048447976042972714028217652392199657870122677085361940416547100605619666");
    xs.emplace_back("646.57075744998934067917908899466389773483433056006707491873238242675958808933605915556193840685637786");

    for (const auto &x : xs) {
        H.L->info("x = {}", x);
        if (auto P = guess(real_t{x.c_str()}, 80)) H.L->info("  {}", format(*P));
        H.L->info("");
    }

    const char *r = "0.1722882583776278670500267959231284336682007863854856624427574750255049273322927690638923632";
    const char *i = "0.5302487364574217190358808797265653491226567421626168710631761419479819886565504921987031543";
    H.L->info("x = {} + {} i", r, i);
    complex_t z{real_t(r), real_t(i)};
    if (auto P = guess(z, 80)) H.L->info("  {}", format(*P));
}
