#include <vb/mp.h>
#include <iostream>

using namespace vb;

int main() {
    real_t::default_precision(100);
    std::cerr << real_t{mpz_int{1} * real_t{1}}.precision() << "\n";
    std::cerr << real_t{real_t{mpz_int{1}} * real_t{1}}.precision() << "\n";
}
