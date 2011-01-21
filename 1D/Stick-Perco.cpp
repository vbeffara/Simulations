/*
 * "Stick Percolation" with sticks following the Cauchy distribution (discrete
 * time for now).
 */

#define NSTEPS 10000
#define STEPSIZE 10000

#include <stdio.h>
#include <stdlib.h>

#include <vb/PRNG.h>

int main() {
  double h,x;
  long i,j;

  h=0;

  for (i=0;i<NSTEPS;i++) {
    for (j=0;j<STEPSIZE;j++) {
      x = 1/drand48();
      if (x>h) {h=x;}
      h -= 1;
      if (h<0) {h=0;}
    }
    printf ("%lf\n",h);
  }
  
  return 0;
}
