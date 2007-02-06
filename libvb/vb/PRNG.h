
/// @file PRNG.h
/// Definitions for the vb::PRNG class

#ifndef __VB_PRNG_H
#define __VB_PRNG_H

#include <math.h>

namespace vb {

  /** A rewindable pseudo-random number generator.
   *
   * The point is to have a bad, but rewindable random number source.
   * The algorithm is just linear congruence, so it iterates a*x+b
   * modulo max, with standard values for them. Rewinding is just a
   * matter of iterating r_a*x+r_b as many times as necessary, and this
   * is very efficiet to do (logarithmic in the number steps).
   *
   * This is very useful to program coupling from the past without
   * having to store the random numbers.
   *
   * Several classical distributions are implemented, see the
   * documentation for a complete list (I add them as I need them ...)
   */

  class PRNG {
  public:
    long max;    ///< The modulo used in the computations.
    
    /** The standard constructor of the vb::PRNG class.
     *
     * Default values of the parameters are the same as in standard Unix
     * rand(), i.e. a=13, b=257, modulo 2**31-1.
     *
     * @param aa The multipplicator.
     * @param bb The shift.
     * @param mm The modulus.
     */
     
    PRNG (long aa=13, long bb=257, long mm=2147483647);

    /** Seed the PRNG with an initial value. */

    void srand (long seed);

    /** Iterate x -> a*x+b, n times.
     *
     * This is done using a dyadic algorithm, so it takes time
     * O(log(n)).
     */

    void iterate (long long a, long long b, long long n);

    /** Rewind the renerator, time1*time2 times.
     *
     * Internally it just calls iterate(r_a,r_b,time1*time2).
     */

    void rewind (long time1, long time2);

    /** Return a pseudo-random number. */

    long rand (void) {
      rdmbuf = (a*rdmbuf+b)%max;
      return (long)rdmbuf;
    }

    /** Return a bernoulli variable in {0,1} */

    int bernoulli (double p=0.5) {
      return rand() < p*(double)max ? 1 : 0;
    }

    /** Return a uniformly distributed real between 0 and range */

    double uniform (double range=1.0) {
      return range * ( (double)(this->rand()) / (double)max );
    }

    /** Return an exponential random variable of parameter lambda */

    double exponential (double lambda=1.0) {
      return -log(this->uniform())/lambda;
    }

    /** Return a Gaussian variable.
     * Caution : it calls rand() twice, if you rewind you should know it.
     */

    double gaussian (double m=0.0, double sigma2=1.0) {
      double modulus = this->exponential();
      double angle = this->uniform(1000.0*3.14159265358979);
      return m + sqrt(sigma2)*modulus*cos(angle);
    }

    /** Return a geometric variable (in Z_+) of parameter p. */

    int geometric (double p) {
      return (int) floor(this->exponential(-log(1-p)));
    }

    /** Return a Poisson variable of parameter lambda. */

    int poisson (double lambda) {
      double u = this->uniform(exp(lambda));
      int k=0, fk=1;
      while (u>0) {
        u -= pow(lambda,k)/fk;
        ++k; fk *= k;
      }
      return k-1;
    }

  private:
    long a,b, r_a,r_b;
    long long rdmbuf;
  };
}

#endif
