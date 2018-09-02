#include <vb/mp.h>

using namespace vb;
using namespace std;
using namespace boost::multiprecision;

template <typename T> void test(const string & s, int n, T z) {
    timing(s, [&] {
        for (int i = 0; i < n; ++i) z = exp(-z);
        return z;
    });
}

int main(int argc, char ** argv) {
    H.init("Testing numerical types", argc, argv, "n=10000");
    int n = H['n'];

    test("Standard float", n, 0.0f);
    test("Standard double", n, 0.0);
    test("Long double", n, 0.0l);

    test("Boost gmp_float (100 digits)", n, number<gmp_float<100>>(0));
    test("Boost gmp_float (200 digits)", n, number<gmp_float<200>>(0));
    test("Boost gmp_float (1000 digits)", n, number<gmp_float<1000>>(0));

    test("Boost mpfr_float (100 digits)", n, number<mpfr_float_backend<100>>(0));
    test("Boost mpfr_float (200 digits)", n, number<mpfr_float_backend<200>>(0));
    test("Boost mpfr_float (1000 digits)", n, number<mpfr_float_backend<1000>>(0));

    mpfr_float::default_precision(100);
    test("Boost mpfr_float (100 digits)", n, number<mpfr_float_backend<0>>(0));
    mpfr_float::default_precision(200);
    test("Boost mpfr_float (200 digits)", n, number<mpfr_float_backend<0>>(0));
    mpfr_float::default_precision(1000);
    test("Boost mpfr_float (1000 digits)", n, number<mpfr_float_backend<0>>(0));
}
