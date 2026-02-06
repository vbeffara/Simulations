#include <vb/util/CLP.h>
#include <vb/util/misc.h>
#include <vb/util/mp.h>

using namespace vb;
using namespace std;
using namespace boost::multiprecision;

template <typename T> void test(const string &s, int n, T z) {
    timing(s, [&] {
        for (int i = 0; i < n; ++i) z = exp(-z);
        return z;
    });
}

template <typename T> struct fmt::formatter<number<T>> : fmt::ostream_formatter {}; // TODO: do this better

auto main(int argc, char **argv) -> int {
    CLP clp(argc, argv, "Testing numerical types");
    int const n = clp.param("n", 10000, "Number of iterations");
    clp.finalize();

    test("Standard float", n, 0.0F);
    test("Standard double", n, 0.0);
    test("Long double", n, 0.0L);

    test("Boost gmp_float (100 digits)", n, number<gmp_float<100>>(0));
    test("Boost gmp_float (200 digits)", n, number<gmp_float<200>>(0));
    test("Boost gmp_float (1000 digits)", n, number<gmp_float<1000>>(0));

    test("Boost mpfr_float (100 digits)", n, number<mpfr_float_backend<100>>(0));
    test("Boost mpfr_float (200 digits)", n, number<mpfr_float_backend<200>>(0));
    test("Boost mpfr_float (1000 digits)", n, number<mpfr_float_backend<1000>>(0));

    default_precision(100);
    test("Boost mpfr_float (100 digits)", n, number<mpfr_float_backend<0>>(0));
    default_precision(200);
    test("Boost mpfr_float (200 digits)", n, number<mpfr_float_backend<0>>(0));
    default_precision(1000);
    test("Boost mpfr_float (1000 digits)", n, number<mpfr_float_backend<0>>(0));

    default_precision(100);
    test("Wrapped real_t (100 digits)", n, real_t(0));
    default_precision(200);
    test("Wrapped real_t (200 digits)", n, real_t(0));
    default_precision(1000);
    test("Wrapped real_t (1000 digits)", n, real_t(0));
}
