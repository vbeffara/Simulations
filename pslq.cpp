/* Implementation of PSLQ in GMP.
  Based on a file Copyright 2004 Paul Zimmermann, LORIA/INRIA Lorraine.

  Usage: pslq [options] < file where file has the following format:

  n - number of floating-point numbers
  y[0]
  ...
  y[n-1]

  Output:
  a polynomial a[0]+a[1]*x+...+a[n-1]*x^(n-1) such that
               a[0]*y[0]+a[1]*y[1]+...+a[n-1]*y[n-1] is a relation.

  Options:
  -p <prec>  set precision to <prec> bits (default 53)
*/

#include <algorithm>
#include <cmath>
#include <gmpxx.h>
#include <iostream>
#include <vector>

void print_relation(std::vector<mpz_class> &A) {
  int first = 1;
  std::cout << "p[x] = ";
  for (int i = A.size() - 1; i >= 0; --i) {
    if (auto s = mpz_sgn(A[i].get_mpz_t()); s != 0) {
      if (!first && s > 0) printf(" + ");
      if (!first && s < 0) printf(" - ");
      if (first && s < 0) printf("- ");
      mpz_out_str(stdout, 10, mpz_class(abs(A[i])).get_mpz_t());
      if (i > 0) { printf(" * x^%u", i); }
      first = 0;
    }
  }
  printf("\n");
}

auto identity(unsigned n) {
  std::vector<std::vector<mpz_class>> A(n, std::vector<mpz_class>(n));
  for (unsigned i = 0; i < A.size(); i++)
    for (unsigned j = 0; j < A[i].size(); j++) A[i][j] = ((i == j) ? 1 : 0);
  return A;
}

/* return m such that |v[m]| is minimal */
int minabs_vector(std::vector<mpz_class> &v) {
  int m = 0;
  for (unsigned i = 1; i < v.size(); i++)
    if (abs(v[i]) < abs(v[m])) m = i;
  return m;
}

/* return m such that |v[m]| is maximal */
int maxabs_mpz_vector(std::vector<mpz_class> &v) {
  int m = 0;
  for (unsigned i = 1; i < v.size(); i++)
    if (abs(v[i]) > abs(v[m])) m = i;
  return m;
}

/* u = max(|v[i]|) for i=0..n-1 */
mpf_class maxabs_vector(std::vector<std::vector<mpf_class>> &v) {
  mpf_class u = abs(v[0][0]);
  for (int i = 1; i < v.size(); i++) u = std::max(u, mpf_class(abs(v[i][i])));
  return u;
}

mpf_class scalprod(const std::vector<mpf_class> &x, const std::vector<mpz_class> &y) {
  mpf_class s = 0;
  for (unsigned i = 0; i < x.size(); i++) { s += y[i] * x[i]; }
  return s;
}

auto PSLQ(const std::vector<mpf_class> &x, double gamma) {
  const int n = x.size();
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
        size_B = std::max(size_B, mpz_sizeinbase(B[j][k].get_mpz_t(), 2));
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
      if (mpf_class norm = pow_gamma * H[i][i]; abs(norm) > maxnorm) {
        m       = i;
        maxnorm = abs(norm);
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
            auto ttt = t.get_si();
            y[j] += ttt * y[i];
            for (k = 0; k < n; k++) {
              A[i][k] -= ttt * A[j][k];
              B[j][k] += ttt * B[i][k];
              size_B = std::max(size_B, mpz_sizeinbase(B[j][k].get_mpz_t(), 2));
            }
            for (k = 0; k <= j; k++) H[i][k] -= ttt * H[j][k];
          } else {
            mpf_class tt = floor(t);
            y[j] += tt * y[i];
            for (k = 0; k < n; k++) {
              A[i][k] -= tt * A[j][k];
              B[j][k] += tt * B[i][k];
              size_B = std::max(size_B, mpz_sizeinbase(B[j][k].get_mpz_t(), 2));
            }
            for (k = 0; k <= j; k++) H[i][k] -= t * H[j][k];
          }
        }
      }
    }

    j = minabs_vector(y);

    if (iter % 100 == 0) {
      auto      k    = maxabs_mpz_vector(y);
      auto      ymax = mpz_sizeinbase(y[k].get_mpz_t(), 2);
      mpf_class u    = 1 / maxabs_vector(H);
      std::cout << "iter=" << iter << "\tM=";
      mpf_out_str(stdout, 10, 3, u.get_mpf_t());
      std::cout << "\tymin=" << mpz_sizeinbase(y[j].get_mpz_t(), 2) << "\tymax=" << ymax << "\tscalprod=";
      mpf_out_str(stdout, 10, 3, scalprod(x, B[j]).get_mpf_t());
      std::cout << std::endl;
    }
  } while (mpz_sizeinbase(y[j].get_mpz_t(), 2) >= size_B);

  if (iter % 100 != 0) {
    mpf_class u = 1 / maxabs_vector(H);
    std::cout << "iter=" << iter << "\tM=";
    mpf_out_str(stdout, 10, 3, u.get_mpf_t());
    std::cout << "\tymin=" << mpz_sizeinbase(y[j].get_mpz_t(), 2) << "\tymax=" << k << "\tscalprod=";
    mpf_out_str(stdout, 10, 3, scalprod(x, B[j]).get_mpf_t());
    std::cout << std::endl;
  }

  return B[j];
}

int main(int argc, char *argv[]) {
  int prec = 53;
  if (argc >= 3 && strcmp(argv[1], "-p") == 0) { prec = atoi(argv[2]); }
  mpf_set_default_prec(prec);
  const double gamma = 1.15470053837925168416; /* sqrt(4/3) rounded up */
  std::cout << "Using precision=" << prec << ", gamma=" << gamma << std::endl;

  int n;
  scanf("%u\n", &n);

  std::vector<mpf_class> x(n);
  for (unsigned i = 0; i < n; i++) {
    mpf_inp_str(x[i].get_mpf_t(), stdin, 10);
    printf("x[%u]=", i);
    mpf_out_str(stdout, 10, 10, x[i].get_mpf_t());
    printf("\n");
  }

  auto rel = PSLQ(x, gamma);
  std::cout << "---" << std::endl;
  print_relation(rel);
}
