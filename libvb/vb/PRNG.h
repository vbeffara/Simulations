/// @file
/// Definitions for the vb::PRNG class

#ifndef __VB_PRNG_H
#define __VB_PRNG_H

#include <vb/common.h>

#include <boost/random.hpp>

namespace vb {
  template <typename G> class PRNG_base : public G {
    public:
      PRNG_base (unsigned long s=0) { if (s) G::seed(s); }

      double bernoulli (double p=0.5) {
        boost::bernoulli_distribution<> dist(p);
        boost::variate_generator<G&, boost::bernoulli_distribution<> > die (*this, dist);
        return die();
      }

      double exponential (double lambda=1.0) {
        boost::exponential_distribution<> dist(lambda);
        boost::variate_generator<G&, boost::exponential_distribution<> > die (*this, dist);
        return die();
      }

      double gaussian (double m=0.0, double sigma=1.0) {
        boost::normal_distribution<> dist(m,sigma);
        boost::variate_generator<G&, boost::normal_distribution<> > die (*this, dist);
        return die();
      }

      double geometric (double p=1.0) {
        boost::geometric_distribution<> dist(p);
        boost::variate_generator<G&, boost::geometric_distribution<> > die (*this, dist);
        return die();
      }

      double poisson (double lambda=1.0) {
        boost::poisson_distribution<> dist(lambda);
        boost::variate_generator<G&, boost::poisson_distribution<> > die (*this, dist);
        return die();
      }

      double uniform_real (double min=0.0, double max=1.0) {
        boost::uniform_real<> dist(min, max);
        boost::variate_generator<G&, boost::uniform_real<> > die (*this, dist);
        return die();
      }

      unsigned int uniform_int (unsigned int range) {
        boost::uniform_int<> dist(0, range-1);
        boost::variate_generator<G&, boost::uniform_int<> > die (*this, dist);
        return die();
      }
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

  extern PRNG prng;
}

#endif
