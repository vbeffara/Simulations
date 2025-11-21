/* Implementation of PSLQ in GMP.
  Based on a file Copyright 2004 Paul Zimmermann, LORIA/INRIA Lorraine.

  Usage: pslq [-p precbits(=53)] < file where file has the following format:

  n - number of floating-point numbers
  y[0]
  ...
  y[n-1]
*/

#include <algorithm>
#include <cmath>
#include <gmpxx.h>
#include <iomanip>
#include <iostream>
#include <vb/util/CLP.h>
#include <vb/util/mp.h>
#include <vector>

using vb::real_t;

void print_relation(const std::vector<mpz_class> &A) {
  int first = 1;
  std::cout << "p[x] = ";
  for (int i = A.size() - 1; i >= 0; --i) {
    if (auto s = sgn(A[i]); s != 0) {
      if (!first && s > 0) std::cout << " + ";
      if (!first && s < 0) std::cout << " - ";
      if (first && s < 0) std::cout << "- ";
      std::ostringstream os;
      os << abs(A[i]);
      std::cout << os.str();
      if (i > 0) { std::cout << " * x^" << i; }
      first = 0;
    }
  }
  std::cout << std::endl;
}

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

auto PSLQ(const std::vector<mpf_class> &x, double gamma = 1.16, bool verbose = false) {
  const int  n = x.size();
  long       j, k, iter = 0;
  size_t     size_B = 0;
  const auto prec   = x[0].get_prec();

  // Normalize and truncate input
  mpf_class t0 = 0;
  for (int k = 0; k < n; k++) t0 += x[k] * x[k];
  t0 = sqrt(t0);
  std::vector<mpz_class> y(n);
  for (int k = 0; k < n; k++) y[k] = x[k] * ((1_mpz << prec) / t0);

  // compute s
  std::vector<mpf_class> s(n);
  for (int k = n - 1; k >= 0; k--) {
    s[k] = x[k] * x[k];
    if (k < n - 1) s[k] += s[k + 1];
  }
  for (int k = 0; k < n; k++) s[k] = sqrt(s[k]);

  // Initialize H
  std::vector<std::vector<mpf_class>> H(n, std::vector<mpf_class>(n));
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

    mpf_class maxnorm   = abs(H[0][0]);
    mpf_class pow_gamma = 1; /* pow_gamma = gamma^i */
    int       m         = 0;
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

auto main(int argc, char **argv) -> int {
  vb::CLP clp(argc, argv, "Testing the PSLQ algorithm");
  auto    prec    = clp.param("p", 53, "Precision in bits");
  auto    gamma   = clp.param("g", 1.16, "Gamma parameter");
  auto    d       = clp.param("d", 2, "Degree of polynomial");
  auto    z       = clp.param("x", "1.6180339887498948482045868343656381177203091798057628621354486227052604628189024497072072041893911374",
                              "Input number x");
  auto    verbose = clp.flag("v", "Verbose output");
  clp.finalize();

  mpf_set_default_prec(prec);
  mpf_class zz(z);

  spdlog::info("Precision: {} bits, gamma = {}, dimension = {}", prec, gamma, d+1);
  std::vector<mpf_class> x(1,1);
  for (unsigned i = 1; i <= d; i++) {
    x.push_back (x.back() * zz);
    std::stringstream ss;
    ss << std::setprecision(10) << x[i];
    spdlog::info("x[{}] ≃ {}", i, ss.str());
  }

  auto              rel = PSLQ(x, gamma, verbose);
  std::stringstream scalprod_str;
  scalprod_str << scalprod(x, rel);
  spdlog::info("Final dot product = {}", scalprod_str.str());
  print_relation(rel);
}
