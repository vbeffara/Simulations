/// @file
/// Definitions for the vb::PRNG class

#ifndef __VB_PRNG_H
#define __VB_PRNG_H

#include <vb/common.h>

#include <boost/random/mersenne_twister.hpp>

namespace vb {
  /** A general-purpose pseudo-random number generator framework.
   *
   * Several classical distributions are implemented, see the
   * documentation for a complete list (I add them as I need them ...). 
   * Some of them are inlined for better performance.
   */

  template <typename G = boost::mt19937> class PRNG_base : public G {
    public:
      unsigned long max; ///< The range of the operator() method.

      /** Initialize the PRNG from a seed.
       *
       * @param seed The initialization seed.
       */

      virtual void srand (unsigned long seed) =0;

      /** Produce a pseudo-random number generator.
       *
       * If possible, the implementation should be mostly inlined.
       */

      virtual unsigned long operator() () =0;

      /** Return a bernoulli variable in {0,1}
       *
       * @param p The parameter of the distribution.
       */

      int bernoulli (double p) {
        return (*this)() < p*(double)max ? 1 : 0;
      }

      /** Return a uniformly distributed real between 0 and range
       *
       * @param range The length of the range.
       */

      double uniform (double range=1.0) {
        return range * ( (double)(*this)() / (double)max );
      }

      /** Return an exponential random variable of parameter lambda.
       *
       * @param lambda The parameter of the distribution.
       */

      double exponential (double lambda=1.0) {
        return -log(uniform())/lambda;
      }

      /** Return a Gaussian variable.
       *
       * Caution : it calls (*this)() twice, if you rewind you should know it.
       *
       * @param m      The mean of the distribution.
       * @param sigma2 The variance of the distribution.
       */

      double gaussian (double m=0.0, double sigma2=1.0);

      /** Return a geometric variable (in Z_+).
       *
       * @param p The parameter of the distribution.
       */

      int geometric (double p) {
        return (int) floor(exponential(-log(1-p)));
      }

      /** Return a Poisson variable of parameter lambda.
       *
       * @param lambda The parameter of the distribution.
       */

      int poisson (double lambda = 1.0);
  };

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
   */

  class PRNG_Rewindable : public PRNG_base <> {
    public:
      /** The standard constructor.
       *
       * Default values of the parameters are the same as in standard Unix
       * (*this)(), i.e. a=13, b=257, modulo 2**31-1.
       *
       * @param aa The multipplicator.
       * @param bb The shift.
       * @param mm The modulus.
       */

      PRNG_Rewindable (long aa=13, long bb=257, long mm=2147483647);

      /** Seed the PRNG with an initial value.
       *
       * @param seed The initialization value to use.
       */

      void srand (unsigned long seed = 0);

      /** Iterate x -> a*x+b, n times.
       *
       * This is done using a dyadic algorithm, so it takes time
       * O(log(n)).
       *
       * @param a The multiplicator.
       * @param b The shift.
       * @param n The number of iterations to perform.
       */

      void iterate (long long a, long long b, long long n);

      /** Rewind the renerator, time1*time2 times.
       *
       * Internally it just calls iterate(r_a,r_b,time1*time2).
       *
       * @param time1 The first factor of the number of iterations.
       * @param time2 The second factor of the number of iterations.
       */

      void rewind (long time1, long time2);

      /** Return a pseudo-random number. */

      unsigned long operator() () {
        rdmbuf = (a*rdmbuf+b)%max;
        return (unsigned long) rdmbuf;
      }

    private:
      long a,b, r_a,r_b;
      long long rdmbuf;
  };

  class PRNG_MT : public PRNG_base <> {
    public:
      PRNG_MT (unsigned long seed = 0); ///< The standard constructor.

      /** Initialize the PRNG from an integer seed.
       * 
       * @param seed The initialization seed.
       */

      void srand (unsigned long seed = 0);

      /// Return an integer between 0 and max.

      unsigned long operator() () { return gen(); }

    private:
      int mti;
      boost::mt19937 gen;
  };

  /** The default pseudo-random number generator.
   *
   * It uses the Mersenne twister engine and implements all the
   * distributions provided by PRNG_template.
   *
   * If you can, use this one. On my system (PPC G4), it is faster than
   * the standard rand() and lrand48() system functions, in addition to
   * being a better random number generator.
   */

  typedef PRNG_MT PRNG;

  extern PRNG prng; ///< A global PRNG instance for convenience. */

  template <typename G> double PRNG_base<G>::gaussian (double m, double sigma2) {
    double modulus = exponential();
    double angle = uniform(1000.0*3.14159265358979);
    return m + sqrt(sigma2)*modulus*cos(angle);
  }
}

#endif
