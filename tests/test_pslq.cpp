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

auto identity(unsigned n) {
  std::vector<std::vector<mpz_class>> A(n, std::vector<mpz_class>(n));
  for (unsigned i = 0; i < A.size(); i++) A[i][i] = 1;
  return A;
}

int argmin_abs(const std::vector<mpz_class> &v) {
  int m = 0;
  for (unsigned i = 1; i < v.size(); i++)
    if (abs(v[i]) < abs(v[m])) m = i;
  return m;
}

int argmax_abs(const std::vector<mpz_class> &v) {
  int m = 0;
  for (unsigned i = 1; i < v.size(); i++)
    if (abs(v[i]) > abs(v[m])) m = i;
  return m;
}

auto nbits(const mpz_class &v) { return mpz_sizeinbase(v.get_mpz_t(), 2); }

mpf_class max_abs_diag(const std::vector<std::vector<mpf_class>> &v) {
  mpf_class u = abs(v[0][0]);
  for (int i = 1; i < v.size(); i++) u = std::max(u, mpf_class(abs(v[i][i])));
  return u;
}

mpf_class scalprod(const std::vector<mpf_class> &x, const std::vector<mpz_class> &y) {
  mpf_class s = x[0] * y[0];
  for (unsigned i = 1; i < x.size(); i++) { s += x[i] * y[i]; }
  return s;
}

auto PSLQ(const std::vector<mpf_class> &x, bool verbose = false, double gamma = 1.16) {
  const int  n = x.size();
  long       j, k, iter = 0;
  size_t     size_B = 0;
  const auto prec   = x[0].get_prec();

  // Normalize and truncate input
  mpf_class t0 = x[0] * x[0];
  for (int k = 1; k < n; k++) t0 += x[k] * x[k];
  t0 = sqrt(t0);
  std::vector<mpz_class> y(n);
  for (int k = 0; k < n; k++) y[k] = x[k] * ((1_mpz << prec) / t0);

  // compute s
  std::vector<mpf_class> s(n, mpf_class(0, prec));
  for (int k = n - 1; k >= 0; k--) {
    s[k] = x[k] * x[k];
    if (k < n - 1) s[k] += s[k + 1];
  }
  for (int k = 0; k < n; k++) s[k] = sqrt(s[k]);

  // Initialize H
  std::vector<std::vector<mpf_class>> H(n, std::vector<mpf_class>(n, mpf_class(0, prec)));
  for (int j = 0; j < n - 1; j++) {
    H[j][j] = s[j + 1] / s[j];
    for (int i = j + 1; i < n; i++) H[i][j] = -(x[i] * x[j]) / (s[j] * s[j + 1]);
  }

  // step 4: Hermite reduce H
  auto A = identity(n), B = identity(n);
  for (int i = 1; i < n; i++) {
    for (int j = i - 1; j >= 0; j--) {
      mpf_class t = floor(H[i][j] / H[j][j] + 0.5);
      y[j] += t * y[i];
      for (k = 0; k <= j; k++) H[i][k] -= t * H[j][k];
      for (k = 0; k < n; k++) {
        A[i][k] -= t * A[j][k];
        B[j][k] += t * B[i][k];
        size_B = std::max(size_B, nbits(B[j][k]));
      }
    }
  }

  do {
    iter++;

    mpf_class maxnorm = abs(H[0][0]), pow_gamma(1, prec);
    int       m       = 0;
    for (int i = 1; i < n - 1; i++) {
      pow_gamma *= gamma;
      if (mpf_class norm = pow_gamma * abs(H[i][i]); norm > maxnorm) {
        m       = i;
        maxnorm = norm;
      }
    }

    std::swap(y[m], y[m + 1]);
    std::swap(A[m], A[m + 1]);
    std::swap(B[m], B[m + 1]);
    std::swap(H[m], H[m + 1]);

    if (m < n - 2) {
      mpf_class t0 = sqrt(H[m][m] * H[m][m] + H[m][m + 1] * H[m][m + 1]);
      mpf_class t1 = H[m][m] / t0;
      mpf_class t2 = H[m][m + 1] / t0;
      for (int i = m; i < n; i++) {
        auto H_i__m_ = H[i][m];
        H[i][m]      = t1 * H[i][m] + t2 * H[i][m + 1];
        H[i][m + 1]  = t1 * H[i][m + 1] - t2 * H_i__m_;
      }
    }

    for (int i = m + 1; i < n; i++) {
      for (int j = std::min(i - 1, m + 1); j >= 0; j--) {
        if (abs(H[i][j]) < abs(H[j][j]) / 2) continue;
        if (mpf_class t = floor(H[i][j] / H[j][j] + .5); t != 0) {
          if (t.fits_slong_p()) {
            long tt = t.get_si();
            y[j] += tt * y[i];
            for (k = 0; k < n; k++) {
              A[i][k] -= tt * A[j][k];
              B[j][k] += tt * B[i][k];
              size_B = std::max(size_B, nbits(B[j][k]));
            }
            for (k = 0; k <= j; k++) H[i][k] -= tt * H[j][k];
          } else {
            mpf_class tt = floor(t);
            y[j] += tt * y[i];
            for (k = 0; k < n; k++) {
              A[i][k] -= tt * A[j][k];
              B[j][k] += tt * B[i][k];
              size_B = std::max(size_B, nbits(B[j][k]));
            }
            for (k = 0; k <= j; k++) H[i][k] -= t * H[j][k];
          }
        }
      }
    }

    j = argmin_abs(y);

    if (verbose) {
      auto              k    = argmax_abs(y);
      auto              ymax = nbits(y[k]);
      mpf_class         u    = 1 / max_abs_diag(H);
      std::stringstream u_str;
      u_str << std::setprecision(3) << u;
      std::stringstream scalprod_str;
      scalprod_str << scalprod(x, B[j]);
      spdlog::info("iter = {}\tM = {}\tymin = {}\tymax = {}\tdotprod = {}", iter, u_str.str(), nbits(y[j]), ymax, scalprod_str.str());
    }
  } while (nbits(y[j]) >= size_B);

  return B[j];
}

auto guess(mpf_class z, int d, bool verbose = false) {
  auto prec = z.get_prec();
  if (verbose) spdlog::info("Precision: {} bits, dimension = {}", prec, d + 1);

  std::vector<mpf_class> x(1, mpf_class(1, prec));
  for (unsigned i = 1; i <= d; i++) {
    x.push_back(x.back() * z);
    if (verbose) {
      std::stringstream ss;
      ss << std::setprecision(10) << x.back();
      spdlog::info("x[{}] ≃ {}", i, ss.str());
    }
  }

  auto rel = PSLQ(x, verbose);
  if (rel.back() < 0)
    for (auto &x : rel) x = -x;
  return rel;
}

vb::Polynomial<vb::mpz_int> guess(vb::real_t z, int d, bool verbose = false) {
  mpf_class                zz(boost::multiprecision::mpf_float(z).backend().data(), 3.322 * z.precision());
  auto                     rel = guess(zz, d, verbose);
  std::vector<vb::mpz_int> vec;
  for (const auto &r : rel) vec.push_back(vb::mpz_int(r.get_mpz_t()));
  return vb::Polynomial<vb::mpz_int>(vec);
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
    if (abs(zz - z) < pow(vb::real_t{10, z.precision()}, 5 - int(nd))) return P;
  }
  return {};
}

auto main(int argc, char **argv) -> int {
  vb::CLP clp(argc, argv, "Testing the PSLQ algorithm");
  auto    d       = clp.param("d", 2, "Degree of polynomial");
  auto    x       = clp.param("x", "1.6180339887498948482045868343656381177203091798057628621354486227052604628189024497072072041893911374",
                              "Input number x");
  auto    verbose = clp.flag("v", "Verbose output");
  clp.finalize();

  vb::real_t z(x, x.size());
  spdlog::info("x = {}", x);
  if (auto P = guess(z); P) spdlog::info("PSLQ: R[z] = {}", *P);
  if (auto P = vb::guess(z, z.precision()); P) spdlog::info("LLL:  Q[z] = {}", *P);
}
