#include <vb/mp.h>
#include <iostream>

using namespace vb;
using namespace std;

int main() {
    real_t::default_precision(100);
    int nd = 80;

    real_t x{"0.9162918442410306144165008200767499077603397502333144975769802641182380808885019256331544308341889255"};
    std::cerr << "x = " << setprecision(x.precision()) << x << "[" << x.precision() << "]\n";

    mpz_int m = pow(mpz_int(10), nd * 2 / 3);
    std::cerr << "m = " << m << "\n";

    real_t xm{x * m}; // BUG
    // real_t xm{real_t{m} * x}; // FIX
    std::cerr << "xm precision = " << xm.precision() << "\n";
}
