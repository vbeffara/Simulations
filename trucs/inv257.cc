/*
 * inverse 257 modulo 2**31-1
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream.h>
#include <time.h>
#include <math.h>
#include <gmp.h>
#include <gmpxx.h>

int main (void) {
  mpz_t nombre,modulo,inverse;

  mpz_init_set_ui (nombre,1103515245);
  mpz_init_set_ui (modulo,2147483648u);
  mpz_init (inverse);

  mpz_invert (inverse,nombre,modulo);

  mpz_out_str (stdout, 10, inverse);
  printf ("\n");
}


  // Calcul
