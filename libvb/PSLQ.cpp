#include <vb/math/NumberTheory.h>

namespace vb {
  Polynomial<mpz_int> v2p(const std::vector<mpz_class> &rel) {
    std::vector<vb::mpz_int> vec;
    for (const auto &r : rel) vec.push_back(vb::mpz_int(r.get_mpz_t()));
    return vb::Polynomial<vb::mpz_int>(vec);
  }

  inline auto nbits(const mpz_class &v) { return mpz_sizeinbase(v.get_mpz_t(), 2); }

  inline auto nbits(const mpz_int &v) { return mpz_sizeinbase(v.backend().data(), 2); }

  std::vector<mpz_class> PSLQ(const std::vector<mpf_class> &x, double gamma) {
    const int  n = x.size();
    long       j, k = 0;
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
    std::vector<std::vector<mpz_class>> A(n, std::vector<mpz_class>(n));
    for (unsigned i = 0; i < A.size(); i++) A[i][i] = 1;
    auto B = A;
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

      j = 0;
      for (unsigned i = 1; i < y.size(); i++)
        if (abs(y[i]) < abs(y[j])) j = i;
    } while (nbits(y[j]) >= size_B);

    return B[j];
  }

  std::vector<mpz_int> PSLQ(const std::vector<real_t> &x, double gamma) {
    const int  n = x.size();
    long       j, k = 0;
    size_t     size_B = 0;
    const auto prec   = x[0].precision();

    // Normalize and truncate input
    vb::real_t t0 = x[0] * x[0];
    for (int k = 1; k < n; k++) t0 += x[k] * x[k];
    t0 = sqrt(t0);
    std::vector<vb::mpz_int> y(n);
    for (int k = 0; k < n; k++) y[k] = vb::mpz_int(x[k] * ((vb::mpz_int(1) << prec) / t0));

    // compute s
    std::vector<vb::real_t> s(n, vb::real_t(0, prec));
    for (int k = n - 1; k >= 0; k--) {
      s[k] = x[k] * x[k];
      if (k < n - 1) s[k] += s[k + 1];
    }
    for (int k = 0; k < n; k++) s[k] = sqrt(s[k]);

    // Initialize H
    std::vector<std::vector<vb::real_t>> H(n, std::vector<vb::real_t>(n, vb::real_t(0, prec)));
    for (int j = 0; j < n - 1; j++) {
      H[j][j] = s[j + 1] / s[j];
      for (int i = j + 1; i < n; i++) H[i][j] = -(x[i] * x[j]) / (s[j] * s[j + 1]);
    }

    // step 4: Hermite reduce H
    std::vector<std::vector<vb::mpz_int>> A(n, std::vector<vb::mpz_int>(n));
    for (unsigned i = 0; i < A.size(); i++) A[i][i] = 1;
    auto B = A;
    for (int i = 1; i < n; i++) {
      for (int j = i - 1; j >= 0; j--) {
        vb::real_t t = floor(H[i][j] / H[j][j] + 0.5);
        y[j] += t.convert_to<vb::mpz_int>() * y[i];
        for (k = 0; k <= j; k++) H[i][k] -= t * H[j][k];
        for (k = 0; k < n; k++) {
          A[i][k] -= t.convert_to<vb::mpz_int>() * A[j][k];
          B[j][k] += t.convert_to<vb::mpz_int>() * B[i][k];
          size_B = std::max(size_B, nbits(B[j][k]));
        }
      }
    }

    do {
      vb::real_t maxnorm = abs(H[0][0]), pow_gamma(1, prec);
      int        m       = 0;
      for (int i = 1; i < n - 1; i++) {
        pow_gamma *= gamma;
        if (vb::real_t norm = pow_gamma * abs(H[i][i]); norm > maxnorm) {
          m       = i;
          maxnorm = norm;
        }
      }

      std::swap(y[m], y[m + 1]);
      std::swap(A[m], A[m + 1]);
      std::swap(B[m], B[m + 1]);
      std::swap(H[m], H[m + 1]);

      if (m < n - 2) {
        vb::real_t t0 = sqrt(H[m][m] * H[m][m] + H[m][m + 1] * H[m][m + 1]);
        vb::real_t t1 = H[m][m] / t0;
        vb::real_t t2 = H[m][m + 1] / t0;
        for (int i = m; i < n; i++) {
          auto H_i__m_ = H[i][m];
          H[i][m]      = t1 * H[i][m] + t2 * H[i][m + 1];
          H[i][m + 1]  = t1 * H[i][m + 1] - t2 * H_i__m_;
        }
      }

      for (int i = m + 1; i < n; i++) {
        for (int j = std::min(i - 1, m + 1); j >= 0; j--) {
          if (abs(H[i][j]) < abs(H[j][j]) / 2) continue;
          if (vb::real_t t = floor(H[i][j] / H[j][j] + .5); t != 0) {
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
              vb::real_t tt = floor(t);
              y[j] += tt.convert_to<vb::mpz_int>() * y[i];
              for (k = 0; k < n; k++) {
                A[i][k] -= tt.convert_to<vb::mpz_int>() * A[j][k];
                B[j][k] += tt.convert_to<vb::mpz_int>() * B[i][k];
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
} // namespace vb