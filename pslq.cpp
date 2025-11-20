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

mpf_t **init_mpf_matrix(unsigned long n) {
  mpf_t       **A;
  unsigned long i, j;

  A = (mpf_t **)malloc(n * sizeof(mpf_t *));
  for (i = 0; i < n; i++) {
    A[i] = (mpf_t *)malloc(n * sizeof(mpf_t));
    for (j = 0; j < n; j++) {
      mpf_init(A[i][j]);
      mpf_set_ui(A[i][j], 0);
    }
  }
  return A;
}

mpz_t **init_mpz_matrix(unsigned long n) {
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

void set_identity(mpz_t **A, unsigned long n) {
  unsigned long i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) mpz_set_ui(A[i][j], (i == j) ? 1 : 0);
}

/* t <- nearest_integer (u) */
void mpf_nint(mpf_t t, mpf_t u) {
  mpf_set_d(t, 0.5);
  mpf_add(t, u, t);
  mpf_floor(t, t);
}

int mpf_cmpabs(mpf_t a, mpf_t b) {
  int res, sa, sb;

  sa = mpf_sgn(a);
  sb = mpf_sgn(b);

  if (sa >= 0 && sb >= 0)
    res = mpf_cmp(a, b);
  else if (sa <= 0 && sb <= 0)
    res = -mpf_cmp(a, b);
  else /* signs differ */
  {
    if (sa < 0)
      mpf_neg(a, a);
    else
      mpf_neg(b, b);
    res = mpf_cmp(a, b);
    if (sa < 0)
      mpf_neg(a, a);
    else
      mpf_neg(b, b);
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
void maxabs_vector(mpf_t u, mpf_t **v, unsigned long n) {
  unsigned long i;

  mpf_abs(u, v[0][0]);
  for (i = 1; i < n; i++)
    if (mpf_cmpabs(v[i][i], u) > 0) mpf_abs(u, v[i][i]);
}

mp_exp_t mpf_get_exp(mpf_t a) {
  mp_exp_t e;

  mpf_get_d_2exp(&e, a);
  return e;
}

/* s <- x[0]*y[0]+...+x[n-1]*y[n-1] */
void scalprod(mpf_t s, std::vector<mpf_class> &x, std::vector<mpz_class> &y, unsigned long n) {
  unsigned long i;
  mpf_t         t;

  mpf_init2(t, mpf_get_prec(x[0].get_mpf_t()));
  mpf_set_ui(s, 0);
  for (i = 0; i < n; i++) {
    mpf_set_z(t, y[i].get_mpz_t());
    mpf_mul(t, x[i].get_mpf_t(), t);
    mpf_add(s, s, t);
  }
  mpf_clear(t);
}

/*
  x0[0..n-1] is the input vector
  put the found relation in y[0..n-1]
  gam is the 'gamma' coefficient (see references [1,2])
  verbose is the verbosity level (default 0)
  output_B is the file for the output matrix B (NULL if no file)
  report - log lines are given each 'report' iterations
 */
auto pslq(std::vector<mpf_class> &x0, unsigned long n, double gam) {
  const int verbose = 3;
  const int report  = 100;
  mpz_t     tt, *Bp;
  mpf_t    *Hp;
  mpf_t     t, u, v, t0, t1, t2, t3;
  long      i, j, k, m, iter = 0;
  mpf_t     gamma, pow_gamma, norm, maxnorm;
  size_t    size_B0 = 0, size_B = 0, size;

  auto prec0 = mpf_get_prec(x0[0].get_mpf_t());
  auto prec  = prec0;

  std::vector<mpz_class> y(n);
  auto                   A = init_mpz_matrix(n);
  auto                   B = init_mpz_matrix(n);
  std::vector<mpf_class> s(n);
  std::vector<mpf_class> x = x0;
  auto                   H = init_mpf_matrix(n);
  mpf_init(t);
  mpf_init(u);
  mpf_init(v);
  mpf_init(t0);
  mpf_init(t1);
  mpf_init(t2);
  mpf_init(t3);
  mpz_init(tt);
  mpf_init2(gamma, 53);
  mpf_init2(pow_gamma, 53);
  mpf_init2(norm, 53);
  mpf_init2(maxnorm, 53);

  mpf_set_d(gamma, gam);

  /* compute the norm of x */
  mpf_set_ui(t0, 0);
  for (k = 0; k < n; k++) {
    mpf_mul(t1, x0[k].get_mpf_t(), x0[k].get_mpf_t());
    mpf_add(t0, t0, t1);
  }
  mpf_sqrt(t0, t0);

  /* if there are pre-computed matrices A and B:
     - x = B*x
     - y = x0/t0, then y = B*y
  */

  /* step 1 */
  set_identity(A, n);
  set_identity(B, n);

  /* step 2 */
  /* normalize y[] so that it has prec bits */
  for (k = 0; k < n; k++) {
    mpf_div(t, x[k].get_mpf_t(), t0); /* |t| < 1 */
    mpf_mul_2exp(t, t, prec);
    mpz_set_f(y[k].get_mpz_t(), t);
  }

  /* compute s[] from x0[] */
  for (k = n - 1; k >= 0; k--) {
    mpf_mul(s[k].get_mpf_t(), x0[k].get_mpf_t(), x0[k].get_mpf_t());
    if (k < n - 1) mpf_add(s[k].get_mpf_t(), s[k].get_mpf_t(), s[k + 1].get_mpf_t());
  }
  for (k = 0; k < n; k++) mpf_sqrt(s[k].get_mpf_t(), s[k].get_mpf_t());

  /* step 3 */
  for (j = 0; j < n - 1; j++) {
    mpf_div(H[j][j], s[j + 1].get_mpf_t(), s[j].get_mpf_t());
    for (i = j + 1; i < n; i++) {
      mpf_mul(H[i][j], s[j].get_mpf_t(), s[j + 1].get_mpf_t());
      mpf_div(H[i][j], x0[i].get_mpf_t(), H[i][j]);
      mpf_mul(H[i][j], H[i][j], x0[j].get_mpf_t());
      mpf_neg(H[i][j], H[i][j]);
    }
  }

  /* step 4: Hermite reduce H, omit for a restart */
  for (i = 1; i < n; i++) {
    for (j = i - 1; j >= 0; j--) {
      mpf_div(u, H[i][j], H[j][j]);
      mpf_nint(t, u);
      mpz_set_f(tt, t);
      mpz_addmul(y[j].get_mpz_t(), tt, y[i].get_mpz_t());
      for (k = 0; k <= j; k++) {
        mpf_mul(u, t, H[j][k]);
        mpf_sub(H[i][k], H[i][k], u);
      }
      for (k = 0; k < n; k++) {
        mpz_submul(A[i][k], tt, A[j][k]);
        mpz_addmul(B[j][k], tt, B[i][k]);
        if ((size = mpz_sizeinbase(B[j][k], 2)) > size_B) size_B = size;
      }
    }
  }

  do {
    iter++;

    /* step 1 */
    mpf_abs(maxnorm, H[0][0]);
    m = 0;                    /* value of i such that gamma^i*|H[i][i]| is maximal */
    mpf_set_ui(pow_gamma, 1); /* pow_gamma = gamma^i */
    for (i = 1; i < n - 1; i++) {
      mpf_mul(pow_gamma, pow_gamma, gamma);
      mpf_mul(norm, pow_gamma, H[i][i]);
      if (mpf_cmpabs(norm, maxnorm) > 0) {
        m = i;
        mpf_abs(maxnorm, norm);
      }
    }

    /* step 2 */
    mpz_swap(y[m].get_mpz_t(), y[m + 1].get_mpz_t());
    Hp       = H[m];
    H[m]     = H[m + 1];
    H[m + 1] = Hp;
    Bp       = A[m];
    A[m]     = A[m + 1];
    A[m + 1] = Bp;
    Bp       = B[m];
    B[m]     = B[m + 1];
    B[m + 1] = Bp;

    /* step 3 */
    if (m < n - 2) {
      mpf_mul(t0, H[m][m], H[m][m]);
      mpf_mul(t1, H[m][m + 1], H[m][m + 1]);
      mpf_add(t0, t0, t1);
      mpf_sqrt(t0, t0);
      mpf_ui_div(t0, 1, t0); /* invert t0 to avoid two divisions */
      mpf_mul(t1, H[m][m], t0);
      mpf_mul(t2, H[m][m + 1], t0);
      for (i = m; i < n; i++) {
        mpf_mul(u, t1, H[i][m]);
        mpf_mul(v, t2, H[i][m + 1]);
        mpf_add(t3, u, v);
        mpf_swap(t3, H[i][m]); /* avoids mpf_set */
        mpf_mul(u, t2, t3);
        mpf_mul(v, t1, H[i][m + 1]);
        mpf_sub(H[i][m + 1], v, u);
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
        exp_hij = mpf_get_exp(H[i][j]);
        exp_hjj = mpf_get_exp(H[j][j]);
        if (exp_hij + 1 < exp_hjj) /* |H[i][j]| < 1/2*|H[j][j]| */
          continue;
        prec_u = exp_hij - exp_hjj + 10;
        mpf_set_prec(u, prec_u);
        mpf_set_prec(t, prec_u);
        mpf_div(u, H[i][j], H[j][j]);
        mpf_nint(t, u);
        if (mpf_cmp_ui(t, 0)) {
          if ((fits = mpf_fits_slong_p(t))) {
            ttt = mpf_get_si(t);
            mpz_addmul_si(y[j].get_mpz_t(), y[i].get_mpz_t(), ttt);
            for (k = 0; k < n; k++) {
              mpz_submul_si(A[i][k], A[j][k], ttt);
              mpz_addmul_si(B[j][k], B[i][k], ttt);
              if ((size = mpz_sizeinbase(B[j][k], 2)) > size_B) size_B = size;
            }
          } else /* should happen rarely */
          {
            mpz_set_f(tt, t);
            mpz_addmul(y[j].get_mpz_t(), tt, y[i].get_mpz_t());
            for (k = 0; k < n; k++) {
              mpz_submul(A[i][k], tt, A[j][k]);
              mpz_addmul(B[j][k], tt, B[i][k]);
              if ((size = mpz_sizeinbase(B[j][k], 2)) > size_B) size_B = size;
            }
          }
          for (k = 0; k <= j; k++) {
            if (fits)
              mpf_mul_si(v, H[j][k], ttt);
            else
              mpf_mul(v, H[j][k], t);
            mpf_sub(H[i][k], H[i][k], v);
          }
        }
      }
    }
    /* reset precision of u and t */
    mpf_set_prec(u, prec);
    mpf_set_prec(t, prec);

    /* step 5 */
    j = minabs_vector(y, n);
    k = maxabs_mpz_vector(y, n);
    k = mpz_sizeinbase(y[k].get_mpz_t(), 2);
    if (verbose && (iter % report == 0)) {
      maxabs_vector(u, H, n - 1);
      mpf_ui_div(u, 1, u);
      printf("iter=%lu M=", iter);
      mpf_out_str(stdout, 10, 3, u);
      printf(" ymin=%lu", mpz_sizeinbase(y[j].get_mpz_t(), 2));
      printf(" ymax=%lu", k);
      printf("\n");
      fflush(stdout);
    }

    /* reduce working precision? */
    if (k + prec / 10 + 20 < prec) {
      prec = k + 10;
      if (verbose >= 2) printf("Reducing precision to %lu\n", prec);
      for (unsigned i = 0; i < n; i++)
        for (unsigned j = 0; j < n; j++) mpf_set_prec(H[i][j], prec);
      mpf_set_prec(t0, prec);
      mpf_set_prec(t1, prec);
      mpf_set_prec(t2, prec);
      mpf_set_prec(t3, prec);
      mpf_set_prec(u, prec);
      mpf_set_prec(v, prec);
      mpf_set_prec(t, prec);
    }

    /* when the norm of B is larger than that of y[], this means that
       neglected bits in the input x[] will give a non-zero contribution,
       so we can stop */
    size = mpz_sizeinbase(y[j].get_mpz_t(), 2);
  } while (size >= size_B0 + size_B);

  if (!verbose || (iter % report != 0)) {
    maxabs_vector(u, H, n - 1);
    mpf_ui_div(u, 1, u);
    printf("iter=%lu M=", iter);
    mpf_out_str(stdout, 10, 3, u);
  }

  for (unsigned long i = 0; i < n; i++) mpz_set(y[i].get_mpz_t(), B[j][i]);
  return y;
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
    mpf_t tmp;
    mpf_init(tmp);
    mpf_inp_str(tmp, stdin, 10);
    x[i] = mpf_class(tmp);
    printf("x[%u]=", i);
    mpf_out_str(stdout, 10, 3, x[i].get_mpf_t());
    printf("\n");
  }

  auto rel = pslq(x, n, gamma);
  print_relation(rel, n);

  mpf_t s;
  mpf_init(s);
  scalprod(s, x, rel, n);
  printf("scalprod=");
  mpf_out_str(stdout, 10, 3, s);
  printf("\n");
}
