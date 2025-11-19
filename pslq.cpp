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
  -g <gam>   set gamma to <gam> (default 2.0)
  -t <tau>   set tau to <tau> (1/tau^2 = 1/4 + 1/gamma^2)
  -v         verbose mode (prints iterations)
*/

#define VERSION "1.0"

#include "gmp.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>

/* #define DEBUG */

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define OUT(a)                                                                                                                             \
  {                                                                                                                                        \
    mpf_out_str(stdout, 10, 0, a);                                                                                                         \
    printf("\n");                                                                                                                          \
  }
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

void mpf_set_prec_matrix(mpf_t **A, unsigned long n, unsigned long prec) {
  unsigned long i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) mpf_set_prec(A[i][j], prec);
}

void mpf_set_prec_vector(mpf_t *v, unsigned long n, unsigned long prec) {
  unsigned long i;

  for (i = 0; i < n; i++) mpf_set_prec(v[i], prec);
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

/* compute total numbers of bits in relation A[0..n-1] */
size_t size_relation(mpz_t *A, unsigned long n) {
  size_t        s = 0;
  unsigned long i;

  for (i = 0; i < n; i++) s += mpz_sizeinbase(A[i], 2);
  return s;
}

/* print relation A[0..n-1] */
void print_relation(mpz_t *A, unsigned long n) {
  unsigned long i;
  int           s, first = 1;
  FILE         *fp = stdout;

  fprintf(fp, "p=");
  for (i = 0; i < n; i++) {
    if ((s = mpz_sgn(A[i]))) {
      if (!first && s > 0) fprintf(fp, "+");
      if (mpz_cmp_ui(A[i], 1)) {
        mpz_out_str(fp, 10, A[i]);
        fprintf(fp, "*");
      }
      fprintf(fp, "x^%lu", i);
      first = 0;
    }
  }
  fprintf(fp, "\n");
}

void set_identity(mpz_t **A, unsigned long n) {
  unsigned long i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) mpz_set_ui(A[i][j], (i == j) ? 1 : 0);
}

mpf_t *init_mpf_vector(unsigned long n) {
  mpf_t        *v;
  unsigned long i;

  v = (mpf_t *)malloc(n * sizeof(mpf_t));
  for (i = 0; i < n; i++) mpf_init(v[i]);
  return v;
}

void set_mpf_vector(mpf_t *v, mpf_t *w, unsigned long n) {
  unsigned long i;

  for (i = 0; i < n; i++) mpf_set(v[i], w[i]);
}

void set_mpz_vector(mpz_t *v, mpz_t *w, unsigned long n) {
  unsigned long i;

  for (i = 0; i < n; i++) mpz_set(v[i], w[i]);
}

mpz_t *init_mpz_vector(unsigned long n) {
  mpz_t        *v;
  unsigned long i;

  v = (mpz_t *)malloc(n * sizeof(mpz_t));
  for (i = 0; i < n; i++) mpz_init(v[i]);
  return v;
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
unsigned long minabs_vector(mpz_t *v, unsigned long n) {
  unsigned long i, m;

  m = 0;
  for (i = 1; i < n; i++)
    if (mpz_cmpabs(v[i], v[m]) < 0) m = i;
  return m;
}

/* return m such that |v[m]| is maximal */
unsigned long maxabs_mpz_vector(mpz_t *v, unsigned long n) {
  unsigned long i, m;

  m = 0;
  for (i = 1; i < n; i++)
    if (mpz_cmpabs(v[i], v[m]) > 0) m = i;
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
void scalprod(mpf_t s, mpf_t *x, mpz_t *y, unsigned long n) {
  unsigned long i;
  mpf_t         t;

  mpf_init2(t, mpf_get_prec(x[0]));
  mpf_set_ui(s, 0);
  for (i = 0; i < n; i++) {
    mpf_set_z(t, y[i]);
    mpf_mul(t, x[i], t);
    mpf_add(s, s, t);
  }
  mpf_clear(t);
}

/* y <- B * x : y[i] = add(B[i][j]*x[j]) */
void mul_matrix_vector(mpf_t *y, mpz_t **B, mpf_t *x, unsigned long n) {
  size_t        s, maxsize;
  unsigned long i, j;
  mpf_t         t, u;

  /* compute the maximal number of bits of B[i][j] */
  maxsize = 0;
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
      s = mpz_size(B[i][j]);
      if (s > maxsize) maxsize = s;
    }

  mpf_init2(t, maxsize * mp_bits_per_limb);
  mpf_init2(u, mpf_get_prec(x[0]));

  for (i = 0; i < n; i++) /* set y[i] */
  {
    mpf_set_ui(y[i], 0);
    for (j = 0; j < n; j++) {
      mpf_set_z(t, B[i][j]);
      mpf_mul(u, t, x[j]);
      mpf_add(y[i], y[i], u);
    }
  }

  mpf_clear(t);
  mpf_clear(u);
}

/* C <- B * A */
void mul_matrix_matrix(mpz_t **C, mpz_t **B, mpz_t **A, unsigned long n) {
  unsigned long i, j, k;

  for (k = 0; k < n; k++)
    for (i = 0; i < n; i++) {
      mpz_mul(C[k][i], B[k][0], A[0][i]);
      for (j = 1; j < n; j++) mpz_addmul(C[k][i], B[k][j], A[j][i]);
    }
}

/* C <- B * A where B is n x n, A is n x (n-1) */
void mul_matz_matf(mpf_t **C, mpz_t **B, mpf_t **A, unsigned long n) {
  unsigned long i, j, k, prec;
  mpf_t         t, u;

  prec = mpf_get_prec(A[0][0]);
  mpf_init2(t, prec);
  mpf_init2(u, prec);
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) mpf_set_ui(C[i][j], 0);
  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) {
      mpf_set_z(t, B[i][j]);
      for (k = 0; k < n - 1; k++) {
        mpf_mul(u, t, A[j][k]);
        mpf_add(C[i][k], C[i][k], u);
      }
    }
  mpf_clear(t);
  mpf_clear(u);
}

/* B <-> C */
void swap_matrix_z(mpz_t **B, mpz_t **C, unsigned long n) {
  unsigned long i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) mpz_swap(B[i][j], C[i][j]);
}

/* B <-> C */
void swap_matrix_f(mpf_t **B, mpf_t **C, unsigned long n) {
  unsigned long i, j;

  for (i = 0; i < n; i++)
    for (j = 0; j < n; j++) mpf_swap(B[i][j], C[i][j]);
}

/*
  x0[0..n-1] is the input vector
  put the found relation in y[0..n-1]
  gam is the 'gamma' coefficient (see references [1,2])
  verbose is the verbosity level (default 0)
  output_B is the file for the output matrix B (NULL if no file)
  report - log lines are given each 'report' iterations
 */
void pslq(mpz_t *y, mpf_t *x0, unsigned long n, double gam, int verbose, int report) {
  mpz_t **A, **B, tt, *Bp;
  mpf_t **H, *Hp, *s, *x;
  mpf_t   t, u, v, t0, t1, t2, t3;
  long    i, j, k, m, iter = 0, prec, prec0;
  mpf_t   gamma, pow_gamma, norm, maxnorm;
  size_t  size_B0 = 0, size_B = 0, size;

  prec = prec0 = mpf_get_prec(x0[0]);

  A = init_mpz_matrix(n);
  B = init_mpz_matrix(n);
  s = init_mpf_vector(n);
  x = init_mpf_vector(n); /* copy of x0 */
  H = init_mpf_matrix(n);
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
    mpf_mul(t1, x0[k], x0[k]);
    mpf_add(t0, t0, t1);
  }
  mpf_sqrt(t0, t0);

  /* if there are pre-computed matrices A and B:
     - x = B*x
     - y = x0/t0, then y = B*y
  */

  set_mpf_vector(x, x0, n);

  /* step 1 */
  set_identity(A, n);
  set_identity(B, n);

  /* step 2 */
  /* normalize y[] so that it has prec bits */
  for (k = 0; k < n; k++) {
    mpf_div(t, x[k], t0); /* |t| < 1 */
    mpf_mul_2exp(t, t, prec);
    mpz_set_f(y[k], t);
  }

  /* compute s[] from x0[] */
  for (k = n - 1; k >= 0; k--) {
    mpf_mul(s[k], x0[k], x0[k]);
    if (k < n - 1) mpf_add(s[k], s[k], s[k + 1]);
  }
  for (k = 0; k < n; k++) mpf_sqrt(s[k], s[k]);

  /* step 3 */
  for (j = 0; j < n - 1; j++) {
    mpf_div(H[j][j], s[j + 1], s[j]);
    for (i = j + 1; i < n; i++) {
      mpf_mul(H[i][j], s[j], s[j + 1]);
      mpf_div(H[i][j], x0[i], H[i][j]);
      mpf_mul(H[i][j], H[i][j], x0[j]);
      mpf_neg(H[i][j], H[i][j]);
    }
  }

  /* step 4: Hermite reduce H, omit for a restart */
  for (i = 1; i < n; i++) {
    for (j = i - 1; j >= 0; j--) {
      mpf_div(u, H[i][j], H[j][j]);
      mpf_nint(t, u);
      mpz_set_f(tt, t);
      mpz_addmul(y[j], tt, y[i]);
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
    mpz_swap(y[m], y[m + 1]);
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
      for (j = MIN(i - 1, m + 1); j >= 0; j--) {
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
            mpz_addmul_si(y[j], y[i], ttt);
            for (k = 0; k < n; k++) {
              mpz_submul_si(A[i][k], A[j][k], ttt);
              mpz_addmul_si(B[j][k], B[i][k], ttt);
              if ((size = mpz_sizeinbase(B[j][k], 2)) > size_B) size_B = size;
            }
          } else /* should happen rarely */
          {
            mpz_set_f(tt, t);
            mpz_addmul(y[j], tt, y[i]);
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
    k = mpz_sizeinbase(y[k], 2);
    if (verbose && (iter % report == 0)) {
      maxabs_vector(u, H, n - 1);
      mpf_ui_div(u, 1, u);
      printf("iter=%lu M=", iter);
      mpf_out_str(stdout, 10, 3, u);
      printf(" ymin=%lu", mpz_sizeinbase(y[j], 2));
      printf(" ymax=%lu", k);
      printf("\n");
      fflush(stdout);
    }

    /* reduce working precision? */
    if (k + prec / 10 + 20 < prec) {
      prec = k + 10;
      if (verbose >= 2) printf("Reducing precision to %lu\n", prec);
      mpf_set_prec_matrix(H, n, prec);
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
    size = mpz_sizeinbase(y[j], 2);
  } while (size >= size_B0 + size_B);

  if (!verbose || (iter % report != 0)) {
    maxabs_vector(u, H, n - 1);
    mpf_ui_div(u, 1, u);
    printf("iter=%lu M=", iter);
    mpf_out_str(stdout, 10, 3, u);
  }

  set_mpz_vector(y, B[j], n);
}

int main(int argc, char *argv[]) {
  unsigned long n, i;
  mpf_t        *x, s; /* input vector */
  mpz_t        *rel;  /* found relation */
  double        gamma, tau;
  unsigned long prec    = 53;
  int           verbose = 0, report = 100;

  printf("PSLQ %s [powered by GMP %s]\n", VERSION, gmp_version);
  fflush(stdout);
  gamma = 5200308914369309.0 / 4503599627370496.0; /* sqrt(4/3) rounded up */

  while (argc >= 2) {
    if (argc >= 3 && strcmp(argv[1], "-p") == 0) {
      prec = atoi(argv[2]);
      argc -= 2;
      argv += 2;
    } else if (argc >= 3 && strcmp(argv[1], "-r") == 0) {
      report = atof(argv[2]);
      argc -= 2;
      argv += 2;
    } else if (argc >= 3 && strcmp(argv[1], "-g") == 0) {
      gamma = atof(argv[2]);
      argc -= 2;
      argv += 2;
    } else if (argc >= 3 && strcmp(argv[1], "-t") == 0) {
      tau = atof(argv[2]);
      /* tau = 1/sqrt(1/rho^2+1/gamma^2), with rho=2 for reals
         tau^2 = 1/(1/rho^2+1/gamma^2)
         1/rho^2+1/gamma^2 = 1/tau^2
       */
      gamma = 1.0 / sqrt(1.0 / (tau * tau) - 0.25);
      argc -= 2;
      argv += 2;
    } else if (strcmp(argv[1], "-v") == 0) {
      verbose++;
      argc -= 1;
      argv += 1;
    }
  }

  if (verbose >= 2) printf("Using precision=%lu, gamma=%1.20e\n", prec, gamma);

  mpf_set_default_prec(prec);

  scanf("%lu\n", &n);

  x   = init_mpf_vector(n);
  rel = init_mpz_vector(n);
  mpf_init(s);

  for (i = 0; i < n; i++) {
    mpf_inp_str(x[i], stdin, 10);
    if (verbose >= 2) {
      printf("x[%lu]=", i);
      mpf_out_str(stdout, 10, 3, x[i]);
      printf("\n");
    }
  }

  pslq(rel, x, n, gamma, verbose, report);
  print_relation(rel, n);
  scalprod(s, x, rel, n);
  printf("scalprod=");
  mpf_out_str(stdout, 10, 3, s);
  printf(" bitsize=%zu\n", size_relation(rel, n));
}
