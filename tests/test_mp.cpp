#include <vb/util/misc.h>
#include <vb/util/mp.h>

using namespace vb;
using namespace std;
using namespace boost::multiprecision;

template <typename T> void test(Hub &H, const string &s, int n, T z) {
    timing(H, s, [&] {
        for (int i = 0; i < n; ++i) z = exp(-z);
        return z;
    });
}

int main(int argc, char **argv) {
    Hub H("Testing numerical types", argc, argv, "n=10000");
    int n = H['n'];

    test(H, "Standard float", n, 0.0F);
    test(H, "Standard double", n, 0.0);
    test(H, "Long double", n, 0.0L);

    test(H, "Boost gmp_float (100 digits)", n, number<gmp_float<100>>(0));
    test(H, "Boost gmp_float (200 digits)", n, number<gmp_float<200>>(0));
    test(H, "Boost gmp_float (1000 digits)", n, number<gmp_float<1000>>(0));

    test(H, "Boost mpfr_float (100 digits)", n, number<mpfr_float_backend<100>>(0));
    test(H, "Boost mpfr_float (200 digits)", n, number<mpfr_float_backend<200>>(0));
    test(H, "Boost mpfr_float (1000 digits)", n, number<mpfr_float_backend<1000>>(0));

    mpfr_float::default_precision(100);
    test(H, "Boost mpfr_float (100 digits)", n, number<mpfr_float_backend<0>>(0));
    mpfr_float::default_precision(200);
    test(H, "Boost mpfr_float (200 digits)", n, number<mpfr_float_backend<0>>(0));
    mpfr_float::default_precision(1000);
    test(H, "Boost mpfr_float (1000 digits)", n, number<mpfr_float_backend<0>>(0));

    mpfr_float::default_precision(100);
    test(H, "Wrapped real_t (100 digits)", n, real_t(0));
    mpfr_float::default_precision(200);
    test(H, "Wrapped real_t (200 digits)", n, real_t(0));
    mpfr_float::default_precision(1000);
    test(H, "Wrapped real_t (1000 digits)", n, real_t(0));
}
