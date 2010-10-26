/// @file
/// Definitions for the vb::PRNG class

#ifndef __VB_PRNG_H
#define __VB_PRNG_H

#include <vb/common.h>

#include <boost/random/mersenne_twister.hpp>

#include <boost/random/variate_generator.hpp>

#include <boost/random/bernoulli_distribution.hpp>
#include <boost/random/uniform_int.hpp>
#include <boost/random/uniform_real.hpp>

namespace vb {
  template <typename G> class PRNG_base : public G {
    public:
      PRNG_base (unsigned long s=0) { if (s) G::seed(s); }

      int bernoulli (double p) {
        return (*this)() < p*(double)G::max() ? 1 : 0;
      }

      double uniform_real (double min=1.0, double max=0.0) {
        if (min>max) std::swap(min,max);
        boost::uniform_real<> dist(min, max);
        boost::variate_generator<G&, boost::uniform_real<> > die (*this, dist);
        return die();
      }

      unsigned int uniform_int (unsigned int range) {
        boost::uniform_int<> dist(0, range-1);
        boost::variate_generator<G&, boost::uniform_int<> > die (*this, dist);
        return die();
      }

      /** Return an exponential random variable of parameter lambda.
       *
       * @param lambda The parameter of the distribution.
       */

      double exponential (double lambda=1.0) {
        return -log(uniform_real())/lambda;
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

  class PRNG_Rewindable : public PRNG_base <boost::mt19937> {
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

      unsigned long max;

    private:
      long a,b, r_a,r_b;
      long long rdmbuf;
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

  typedef PRNG_base <boost::mt19937> PRNG;

  extern PRNG prng; ///< A global PRNG instance for convenience. */

  template <typename G> double PRNG_base<G>::gaussian (double m, double sigma2) {
    double modulus = exponential();
    double angle = uniform_real(0, 1000.0*3.14159265358979);
    return m + sqrt(sigma2)*modulus*cos(angle);
  }
}

#endif
