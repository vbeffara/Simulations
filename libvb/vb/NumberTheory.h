#pragma once
#include <vb/Polynomial.h>
#include <boost/multiprecision/gmp.hpp>
#include <boost/multiprecision/mpfr.hpp>
#include <optional>

namespace vb {
    using boost::multiprecision::mpf_float;
    using boost::multiprecision::mpz_int;

    std::optional<cln::cl_UP_R> guess(const cln::cl_R & x, int nd);
    std::optional<cln::cl_UP_N> guess_r(const cln::cl_N & x, int nd);

    std::optional<Polynomial<mpz_int>> guess(const mpf_float & x, int nd);

#ifdef UNIT_TESTS
    TEST_CASE("NumberTheory guess functions") {
        cln::default_float_format = cln::float_format(100);
        cln::cl_F z("0.9162918442410306144165008200767499077603397502333144975769802641182380808885019256331544308341889255");
        CHECK(str(*(guess(z, 100))) == "1*z^5 + -3*z^4 + 12*z^3 + -2*z^2 + 1*z + -7");

        cln::cl_F z1("0.1722882583776278670500267959231284336682007863854856624427574750255049273322927690638923632");
        cln::cl_F z2("0.5302487364574217190358808797265653491226567421626168710631761419479819886565504921987031543");
        cln::cl_N zc = cln::complex(z1, z2);
        CHECK(str(*(guess_r(zc, 100))) == "1*z^8 + 6*z^7 + 17*z^6 + 18*z^5 + 25*z^4 + -18*z^3 + 17*z^2 + -6*z + 1");

        CHECK(!guess(cln::pi(z), 100));
    }
#endif
} // namespace vb
