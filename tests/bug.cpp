#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/mpfr.hpp>
#include <iostream>

using boost::multiprecision::mpfr_float;
using boost::multiprecision::mpz_int;

int main() {
    mpfr_float::default_precision(100);
    std::cerr << mpfr_float{mpz_int{1} * mpfr_float{1}}.precision() << "\n";
    std::cerr << mpfr_float{mpfr_float{mpz_int{1}} * mpfr_float{1}}.precision() << "\n";
}
