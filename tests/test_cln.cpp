#include <vb/cpx.h>

using namespace vb;
using namespace std;
using namespace cln;

template <typename T> void test(const string & s, int n, T z) {
    timing(s, [&] {
        for (int i = 0; i < n; ++i) z = exp(-z);
        return z;
    });
}

int main(int argc, char ** argv) {
    H.init("Testing numerical types", argc, argv, "n=100000");
    int n = H['n'];

    test("Standard float", n, 0.0f);
    test("Standard double", n, 0.0);
    test("Long double", n, 0.0l);
    test("CLN short float", n, cl_F("0s0"));
    test("CLN single float", n, cl_F("0f0"));
    test("CLN double float", n, cl_F("0d0"));
    test("CLN long float (100 digits)", n, real_t(0));
    default_float_format = float_format(200);
    test("CLN long float (200 digits)", n, real_t(0));
}
