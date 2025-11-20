/* Implementation of PSLQ in GMP.

  Copyright 2004 Paul Zimmermann, LORIA/INRIA Lorraine.

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version.

  This program is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along
  with this program; see the file COPYING.  If not, write to the Free
  Software Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
  02111-1307, USA.

  References:
  [1] Analysis of PSLQ, an integer relation finding algorithm,
      Helaman R. P. Ferguson, David H. Bailey, Steve Arno, Mathematics of
      Computation, volume 68, pages 351--369, 1999.
  [2] Integer Relation Detection, David H. Bailey, Computing in Science
      & Engineering, volume 2, number 1, pages 24--28, 2000.

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
  -v         verbose mode (prints iterations)
*/

#define VERSION "1.0"

#include <algorithm>
#include <gmp.h>
#include <gmpxx.h>
#include <iostream>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <vector>

#define mpz_addmul_si(a, b, c)                                                                                                             \
  do {                                                                                                                                     \
    if (c >= 0)                                                                                                                            \
      mpz_addmul_ui(a, b, (unsigned long)c);                                                                                               \
    else                                                                                                                                   \
      mpz_submul_ui(a, b, (unsigned long)-c);                                                                                              \
  } while (0)
#define mpz_submul_si(a, b, c)                                                                                                             \
  do {                                                                                                                                     \
    if (c >= 0)                                                                                                                            \
      mpz_submul_ui(a, b, (unsigned long)c);                                                                                               \
    else                                                                                                                                   \
      mpz_addmul_ui(a, b, (unsigned long)-c);                                                                                              \
  } while (0)
#define mpf_mul_si(a, b, c)                                                                                                                \
  do {                                                                                                                                     \
    if (c >= 0)                                                                                                                            \
      mpf_mul_ui(a, b, (unsigned long)c);                                                                                                  \
    else {                                                                                                                                 \
      mpf_mul_ui(a, b, (unsigned long)-c);                                                                                                 \
      mpf_neg(a, a);                                                                                                                       \
    }                                                                                                                                      \
  } while (0)

[[deprecated]] mpz_t **init_mpz_matrix(unsigned long n) {
  mpz_t       **A;
  unsigned long i, j;

  A = (mpz_t **)malloc(n * sizeof(mpz_t *));
  for (i = 0; i < n; i++) {
    A[i] = (mpz_t *)malloc(n * sizeof(mpz_t));
    for (j = 0; j < n; j++) mpz_init(A[i][j]);
  }
  return A;
}

/* print relation A[0..n-1] */
void print_relation(std::vector<mpz_class> &A, unsigned long n) {
  unsigned long i;
  int           s, first = 1;

  printf("\np=");
  for (i = 0; i < n; i++) {
    if ((s = mpz_sgn(A[i].get_mpz_t()))) {
      if (!first && s > 0) printf("+");
      if (mpz_cmp_ui(A[i].get_mpz_t(), 1)) {
        mpz_out_str(stdout, 10, A[i].get_mpz_t());
        printf("*");
      }
      printf("x^%lu", i);
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

/* t <- nearest_integer (t) */
auto mpf_nint(mpf_class &t) {
  mpf_class u = t + 0.5;
  mpf_floor(u.get_mpf_t(), u.get_mpf_t());
  return u;
}

int mpf_cmpabs(mpf_class &a, mpf_class &b) {
  int res, sa, sb;

  sa = mpf_sgn(a.get_mpf_t());
  sb = mpf_sgn(b.get_mpf_t());

  if (sa >= 0 && sb >= 0)
    res = mpf_cmp(a.get_mpf_t(), b.get_mpf_t());
  else if (sa <= 0 && sb <= 0)
    res = -mpf_cmp(a.get_mpf_t(), b.get_mpf_t());
  else /* signs differ */
  {
    if (sa < 0)
      mpf_neg(a.get_mpf_t(), a.get_mpf_t());
    else
      mpf_neg(b.get_mpf_t(), b.get_mpf_t());
    res = mpf_cmp(a.get_mpf_t(), b.get_mpf_t());
    if (sa < 0)
      mpf_neg(a.get_mpf_t(), a.get_mpf_t());
    else
      mpf_neg(b.get_mpf_t(), b.get_mpf_t());
  }

  return res;
}

/* return m such that |v[m]| is minimal */
unsigned long minabs_vector(std::vector<mpz_class> &v, unsigned long n) {
  unsigned long m = 0;
  for (unsigned i = 1; i < v.size(); i++)
    if (mpz_cmpabs(v[i].get_mpz_t(), v[m].get_mpz_t()) < 0) m = i;
  return m;
}

/* return m such that |v[m]| is maximal */
unsigned long maxabs_mpz_vector(std::vector<mpz_class> &v, unsigned long n) {
  unsigned long m = 0;
  for (unsigned i = 1; i < v.size(); i++)
    if (mpz_cmpabs(v[i].get_mpz_t(), v[m].get_mpz_t()) > 0) m = i;
  return m;
}

/* u = max(|v[i]|) for i=0..n-1 */
void maxabs_vector(mpf_class &u, std::vector<std::vector<mpf_class>> &v, unsigned long n) {
  unsigned long i;

  u = abs(v[0][0]);
  for (i = 1; i < n; i++)
    if (mpf_cmpabs(v[i][i], u) > 0) u = abs(v[i][i]);
}

mp_exp_t mpf_get_exp(mpf_t a) {
  mp_exp_t e;

  mpf_get_d_2exp(&e, a);
  return e;
}

/* s <- x[0]*y[0]+...+x[n-1]*y[n-1] */
mpf_class scalprod(std::vector<mpf_class> &x, std::vector<mpz_class> &y) {
  mpf_class s = 0;
  for (unsigned i = 0; i < x.size(); i++) { s += y[i] * x[i]; }
  return s;
}

/*
  x0[0..n-1] is the input vector
  put the found relation in y[0..n-1]
  gam is the 'gamma' coefficient (see references [1,2])
  output_B is the file for the output matrix B (NULL if no file)
  report - log lines are given each 'report' iterations
 */
auto pslq(std::vector<mpf_class> &x0, unsigned long n, double gam) {
  long   i, j, k, iter = 0;
  size_t size_B0 = 0, size_B = 0, size;

  auto prec = mpf_get_prec(x0[0].get_mpf_t());

  std::vector<std::vector<mpf_class>> H(n, std::vector<mpf_class>(n));
  std::vector<mpf_class>              s(n), x(x0);
  std::vector<mpz_class>              y(n);
  mpf_class                           t, u, v, t1, t2, t3, gamma(gam), norm;
  mpz_class                           tt;

  mpf_class t0 = 0;
  for (int k = 0; k < n; k++) t0 += x0[k] * x0[k];
  t0 = sqrt(t0);

  /* step 1 */
  auto A = identity(n), B = identity(n);

  /* step 2 */
  for (int k = 0; k < n; k++) y[k] = x[k] * ((1_mpz << prec) / t0);

  /* compute s[] from x0[] */
  for (int k = n - 1; k >= 0; k--) {
    s[k] = x0[k] * x0[k];
    if (k < n - 1) s[k] += s[k + 1];
  }
  for (int k = 0; k < n; k++) s[k] = sqrt(s[k]);

  /* step 3 */
  for (int j = 0; j < n - 1; j++) {
    H[j][j] = s[j + 1] / s[j];
    for (int i = j + 1; i < n; i++) H[i][j] = -(x0[i] * x0[j]) / (s[j] * s[j + 1]);
  }

  /* step 4: Hermite reduce H, omit for a restart */
  for (int i = 1; i < n; i++) {
    for (int j = i - 1; j >= 0; j--) {
      u  = H[i][j] / H[j][j];
      t  = mpf_nint(u);
      tt = t;
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

    /* step 1 */
    mpf_class maxnorm   = abs(H[0][0]);
    long      m         = 0; /* value of i such that gamma^i*|H[i][i]| is maximal */
    mpf_class pow_gamma = 1; /* pow_gamma = gamma^i */

    for (int i = 1; i < n - 1; i++) {
      pow_gamma *= gamma;
      norm = pow_gamma * H[i][i];
      if (mpf_cmpabs(norm, maxnorm) > 0) {
        m = i;
        maxnorm = abs(norm);
      }
    }

    /* step 2 */
    mpz_swap(y[m].get_mpz_t(), y[m + 1].get_mpz_t());
    std::swap(A[m], A[m + 1]);
    std::swap(B[m], B[m + 1]);
    std::swap(H[m], H[m + 1]);

    /* step 3 */
    if (m < n - 2) {
      mpf_mul(t0.get_mpf_t(), H[m][m].get_mpf_t(), H[m][m].get_mpf_t());
      mpf_mul(t1.get_mpf_t(), H[m][m + 1].get_mpf_t(), H[m][m + 1].get_mpf_t());
      mpf_add(t0.get_mpf_t(), t0.get_mpf_t(), t1.get_mpf_t());
      mpf_sqrt(t0.get_mpf_t(), t0.get_mpf_t());
      mpf_ui_div(t0.get_mpf_t(), 1, t0.get_mpf_t()); /* invert t0 to avoid two divisions */
      mpf_mul(t1.get_mpf_t(), H[m][m].get_mpf_t(), t0.get_mpf_t());
      mpf_mul(t2.get_mpf_t(), H[m][m + 1].get_mpf_t(), t0.get_mpf_t());
      for (i = m; i < n; i++) {
        mpf_mul(u.get_mpf_t(), t1.get_mpf_t(), H[i][m].get_mpf_t());
        mpf_mul(v.get_mpf_t(), t2.get_mpf_t(), H[i][m + 1].get_mpf_t());
        mpf_add(t3.get_mpf_t(), u.get_mpf_t(), v.get_mpf_t());
        mpf_swap(t3.get_mpf_t(), H[i][m].get_mpf_t()); /* avoids mpf_set */
        mpf_mul(u.get_mpf_t(), t2.get_mpf_t(), t3.get_mpf_t());
        mpf_mul(v.get_mpf_t(), t1.get_mpf_t(), H[i][m + 1].get_mpf_t());
        mpf_sub(H[i][m + 1].get_mpf_t(), v.get_mpf_t(), u.get_mpf_t());
      }
    }

    /* step 4 */
    for (i = m + 1; i < n; i++) {
      for (j = std::min(i - 1, m + 1); j >= 0; j--) {
        mp_exp_t exp_hij, exp_hjj, prec_u;
        long     ttt;
        int      fits;
        /* we want to compute the nearest integer to H[i][j]/H[j][j],
           but for that we need only small precision */
        exp_hij = mpf_get_exp(H[i][j].get_mpf_t());
        exp_hjj = mpf_get_exp(H[j][j].get_mpf_t());
        if (exp_hij + 1 < exp_hjj) /* |H[i][j]| < 1/2*|H[j][j]| */
          continue;
        prec_u = exp_hij - exp_hjj + 10;
        mpf_set_prec(u.get_mpf_t(), prec_u);
        mpf_set_prec(t.get_mpf_t(), prec_u);
        mpf_div(u.get_mpf_t(), H[i][j].get_mpf_t(), H[j][j].get_mpf_t());
        t = mpf_nint(u);
        if (mpf_cmp_ui(t.get_mpf_t(), 0)) {
          if ((fits = mpf_fits_slong_p(t.get_mpf_t()))) {
            ttt = mpf_get_si(t.get_mpf_t());
            mpz_addmul_si(y[j].get_mpz_t(), y[i].get_mpz_t(), ttt);
            for (k = 0; k < n; k++) {
              mpz_submul_si(A[i][k].get_mpz_t(), A[j][k].get_mpz_t(), ttt);
              mpz_addmul_si(B[j][k].get_mpz_t(), B[i][k].get_mpz_t(), ttt);
              if ((size = mpz_sizeinbase(B[j][k].get_mpz_t(), 2)) > size_B) size_B = size;
            }
          } else /* should happen rarely */
          {
            mpz_set_f(tt.get_mpz_t(), t.get_mpf_t());
            mpz_addmul(y[j].get_mpz_t(), tt.get_mpz_t(), y[i].get_mpz_t());
            for (k = 0; k < n; k++) {
              mpz_submul(A[i][k].get_mpz_t(), tt.get_mpz_t(), A[j][k].get_mpz_t());
              mpz_addmul(B[j][k].get_mpz_t(), tt.get_mpz_t(), B[i][k].get_mpz_t());
              if ((size = mpz_sizeinbase(B[j][k].get_mpz_t(), 2)) > size_B) size_B = size;
            }
          }
          for (k = 0; k <= j; k++) {
            if (fits)
              mpf_mul_si(v.get_mpf_t(), H[j][k].get_mpf_t(), ttt);
            else
              mpf_mul(v.get_mpf_t(), H[j][k].get_mpf_t(), t.get_mpf_t());
            mpf_sub(H[i][k].get_mpf_t(), H[i][k].get_mpf_t(), v.get_mpf_t());
          }
        }
      }
    }
    /* reset precision of u and t */
    mpf_set_prec(u.get_mpf_t(), prec);
    mpf_set_prec(t.get_mpf_t(), prec);

    /* step 5 */
    j = minabs_vector(y, n);
    k = maxabs_mpz_vector(y, n);
    k = mpz_sizeinbase(y[k].get_mpz_t(), 2);
    if (iter % 100 == 0) {
      maxabs_vector(u, H, n - 1);
      mpf_ui_div(u.get_mpf_t(), 1, u.get_mpf_t());
      printf("iter=%lu M=", iter);
      mpf_out_str(stdout, 10, 3, u.get_mpf_t());
      printf(" ymin=%lu", mpz_sizeinbase(y[j].get_mpz_t(), 2));
      printf(" ymax=%lu", k);
      printf("\n");
      fflush(stdout);
    }

    /* reduce working precision? */
    if (k + prec / 10 + 20 < prec) {
      prec = k + 10;
      printf("Reducing precision to %lu\n", prec);
      for (unsigned i = 0; i < n; i++)
        for (unsigned j = 0; j < n; j++) mpf_set_prec(H[i][j].get_mpf_t(), prec);
      mpf_set_prec(t0.get_mpf_t(), prec);
      mpf_set_prec(t1.get_mpf_t(), prec);
      mpf_set_prec(t2.get_mpf_t(), prec);
      mpf_set_prec(t3.get_mpf_t(), prec);
      mpf_set_prec(u.get_mpf_t(), prec);
      mpf_set_prec(v.get_mpf_t(), prec);
      mpf_set_prec(t.get_mpf_t(), prec);
    }

    /* when the norm of B is larger than that of y[], this means that
       neglected bits in the input x[] will give a non-zero contribution,
       so we can stop */
    size = mpz_sizeinbase(y[j].get_mpz_t(), 2);
  } while (size >= size_B0 + size_B);

  if (iter % 100 != 0) {
    maxabs_vector(u, H, n - 1);
    mpf_ui_div(u.get_mpf_t(), 1, u.get_mpf_t());
    printf("iter=%lu M=", iter);
    mpf_out_str(stdout, 10, 3, u.get_mpf_t());
  }

  return B[j];
}

int main(int argc, char *argv[]) {
  printf("PSLQ %s [powered by GMP %s]\n", VERSION, gmp_version);

  unsigned long prec = 53;
  if (argc >= 3 && strcmp(argv[1], "-p") == 0) { prec = atoi(argv[2]); }
  mpf_set_default_prec(prec);

  const double gamma = 1.15470053837925168416; /* sqrt(4/3) rounded up */

  printf("Using precision=%lu, gamma=%f\n", prec, gamma);

  unsigned long n;
  scanf("%lu\n", &n);

  std::vector<mpf_class> x(n);

  for (unsigned i = 0; i < n; i++) {
    mpf_inp_str(x[i].get_mpf_t(), stdin, 10);
    printf("x[%u]=", i);
    mpf_out_str(stdout, 10, 3, x[i].get_mpf_t());
    printf("\n");
  }

  auto rel = pslq(x, n, gamma);
  print_relation(rel, n);

  printf("scalprod=");
  mpf_out_str(stdout, 10, 3, scalprod(x, rel).get_mpf_t());
  printf("\n");
}
