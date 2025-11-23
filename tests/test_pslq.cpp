// Implementation of PSLQ in GMP.
// Based on a file Copyright 2004 Paul Zimmermann, LORIA/INRIA Lorraine.

#include <algorithm>
#include <cmath>
#include <gmpxx.h>
#include <iomanip>
#include <iostream>
#include <vb/math/NumberTheory.h>
#include <vb/math/Polynomial.h>
#include <vb/util/CLP.h>
#include <vb/util/mp.h>
#include <vector>

vb::Polynomial<vb::mpz_int> guess(mpf_class z, int d) {
  std::vector<mpf_class> x(1, z / z);
  for (unsigned i = 1; i <= d; i++) x.push_back(x.back() * z);
  auto P = vb::v2p(vb::PSLQ(x));
  if (P[d] < 0) P *= -1;
  return P;
}

vb::Polynomial<vb::mpz_int> guess(vb::real_t z, int d) {
  std::vector<vb::real_t> x(1, z / z);
  for (unsigned i = 1; i <= d; i++) x.push_back(x.back() * z);
  vb::Polynomial<vb::mpz_int> P(vb::PSLQ(x));
  if (P[d] < 0) P *= -1;
  return P;
}

std::optional<vb::Polynomial<vb::mpz_int>> guess(vb::real_t z) {
  auto nd = z.precision();
  auto sz = vb::real_t(z, 2 * nd / 3);
  for (int d = 1; d < nd / 10; ++d) {
    auto P = guess(sz, d);
    if (P.degree() == 0) continue;
    auto PP = P.derivative();

    vb::real_t zz = z, oz = z + 1, er = 2;
    while (real(abs(zz - oz)) < real(er)) {
      er = abs(zz - oz);
      if (real(er) < pow(vb::real_t{10, z.precision()}, -5 * int(nd))) er = 0;
      oz = zz;
      zz -= P(zz) / PP(zz);
    }
    if (abs(zz - z) < pow(vb::real_t{10, z.precision()}, 10 - int(nd))) return P;
  }
  return {};
}

auto main(int argc, char **argv) -> int {
  vb::CLP clp(argc, argv, "Testing the PSLQ algorithm");
  auto    x = clp.param("x", "", "Input number x");
  clp.finalize();

  std::vector<std::string> xs;
  if (x == "") {
    xs.emplace_back("0.9162918442410306144165008200767499077603397502333144975769802641182380808885019256331544308341889255");
    xs.emplace_back("1.36602540378443864676372317075293618347140262690519031402790348972596650845440001854057309338");
    xs.emplace_back("0.5877852522924731291687059546390727685976524376431459910722724807572784741623519575085040499");
    xs.emplace_back("0.8090169943749474241022934171828190588601545899028814310677243113526302314094512248536036021");
    xs.emplace_back("798303.3673469387755102040816326530612244897959183673469387755102040816326530612244897959183673469");
    xs.emplace_back("1.12266701157648291040117446489401135452719640061624479423083713099312644668872851836902659616");
    xs.emplace_back("0.63827397417446081629048447976042972714028217652392199657870122677085361940416547100605619666");
    xs.emplace_back("646.57075744998934067917908899466389773483433056006707491873238242675958808933605915556193840685637786");
  } else
    xs.emplace_back(x);

  for (const auto &x : xs) {
    spdlog::info("x = {}", x);
    vb::real_t z(x, x.size());
    // spdlog::info("std::string -> vb::real -> mpf_class -> PSLQ:");
    if (auto P = guess(z); P) spdlog::info("    P_PSLQ(z) = {}", *P);
    // spdlog::info("std::string -> vb::real -> FPLLL:");
    if (auto P = vb::guess(z, z.precision()); P) spdlog::info("    P_LLL(z) =  {}", *P);
  }
}
