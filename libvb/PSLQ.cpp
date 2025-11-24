// Implementation of PSLQ in boost::multiprecision.
// Based on a file Copyright 2004 Paul Zimmermann, LORIA/INRIA Lorraine.

#include <vb/math/NumberTheory.h>

namespace vb {
  inline auto nbits(const mpz_int &v) { return mpz_sizeinbase(v.backend().data(), 2); }

  std::vector<mpz_int> PSLQ(const std::vector<real_t> &x, double gamma) {
    const int  n = x.size();
    long       j, k = 0;
    size_t     size_B = 0;
    const auto prec   = x[0].precision();

    // Normalize and truncate input
    real_t t0 = x[0] * x[0];
    for (int k = 1; k < n; k++) t0 += x[k] * x[k];
    t0 = sqrt(t0);
    std::vector<mpz_int> y(n);
    for (int k = 0; k < n; k++) y[k] = mpz_int(x[k] * ((mpz_int(1) << prec) / t0));

    // compute s
    std::vector<real_t> s(n, real_t(0, prec));
    for (int k = n - 1; k >= 0; k--) {
      s[k] = x[k] * x[k];
      if (k < n - 1) s[k] += s[k + 1];
    }
    for (int k = 0; k < n; k++) s[k] = sqrt(s[k]);

    // Initialize H
    std::vector<std::vector<real_t>> H(n, std::vector<real_t>(n, real_t(0, prec)));
    for (int j = 0; j < n - 1; j++) {
      H[j][j] = s[j + 1] / s[j];
      for (int i = j + 1; i < n; i++) H[i][j] = -(x[i] * x[j]) / (s[j] * s[j + 1]);
    }

    // step 4: Hermite reduce H
    std::vector<std::vector<mpz_int>> A(n, std::vector<mpz_int>(n));
    for (unsigned i = 0; i < A.size(); i++) A[i][i] = 1;
    auto B = A;
    for (int i = 1; i < n; i++) {
      for (int j = i - 1; j >= 0; j--) {
        auto t = (H[i][j] / H[j][j] + 0.5).convert_to<mpz_int>();
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
      real_t maxnorm = abs(H[0][0]), pow_gamma(1, prec);
      int        m       = 0;
      for (int i = 1; i < n - 1; i++) {
        pow_gamma *= gamma;
        if (real_t norm = pow_gamma * abs(H[i][i]); norm > maxnorm) {
          m       = i;
          maxnorm = norm;
        }
      }

      std::swap(y[m], y[m + 1]);
      std::swap(A[m], A[m + 1]);
      std::swap(B[m], B[m + 1]);
      std::swap(H[m], H[m + 1]);

      if (m < n - 2) {
        real_t t0 = sqrt(H[m][m] * H[m][m] + H[m][m + 1] * H[m][m + 1]);
        real_t t1 = H[m][m] / t0;
        real_t t2 = H[m][m + 1] / t0;
        for (int i = m; i < n; i++) {
          auto H_i__m_ = H[i][m];
          H[i][m]      = t1 * H[i][m] + t2 * H[i][m + 1];
          H[i][m + 1]  = t1 * H[i][m + 1] - t2 * H_i__m_;
        }
      }

      for (int i = m + 1; i < n; i++) {
        for (int j = std::min(i - 1, m + 1); j >= 0; j--) {
          if (abs(H[i][j]) < abs(H[j][j]) / 2) continue;
          if (mpz_int t = floor(H[i][j] / H[j][j] + .5).convert_to<mpz_int>(); t != 0) {
            if (abs(t) < (1L << 62)) {
              long tt = t.convert_to<long>();
              y[j] += tt * y[i];
              for (k = 0; k < n; k++) {
                A[i][k] -= tt * A[j][k];
                B[j][k] += tt * B[i][k];
                size_B = std::max(size_B, nbits(B[j][k]));
              }
              for (k = 0; k <= j; k++) H[i][k] -= tt * H[j][k];
            } else {
              y[j] += t.convert_to<mpz_int>() * y[i];
              for (k = 0; k < n; k++) {
                A[i][k] -= t.convert_to<mpz_int>() * A[j][k];
                B[j][k] += t.convert_to<mpz_int>() * B[i][k];
                size_B = std::max(size_B, nbits(B[j][k]));
              }
              for (k = 0; k <= j; k++) H[i][k] -= t * H[j][k];
            }
          }
        }
      }

      j = 0;
      for (unsigned i = 1; i < y.size(); i++)
        if (abs(y[i]) < abs(y[j])) j = i;
    } while (nbits(y[j]) >= size_B);

    return B[j];
  }

  std::optional<Polynomial<mpz_int>> guess_PSLQ(real_t z) {
    auto                    nd = z.precision();
    auto                    sz = real_t(z, 2 * nd / 3);
    std::vector<real_t> pows(1, sz / sz);
    for (int d = 1; d < nd / 10; ++d) {
      pows.push_back(pows.back() * sz);
      Polynomial<mpz_int> P(PSLQ(pows));
      if (P.degree() == 0) continue;
      auto PP = P.derivative();

      real_t zz = z, oz = z + 1, er = 2;
      while (real(abs(zz - oz)) < real(er)) {
        er = abs(zz - oz);
        if (real(er) < pow(real_t{10, z.precision()}, -5 * int(nd))) er = 0;
        oz = zz;
        zz -= P(zz) / PP(zz);
      }
      if (abs(zz - z) < pow(real_t{10, z.precision()}, 10 - int(nd))) {
        if (P[d] < 0) P *= -1;
        return P;
      }
    }
    return {};
  }
} // namespace vb