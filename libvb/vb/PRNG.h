/// @file
/// Definitions for the vb::PRNG class

#ifndef __VB_PRNG_H
#define __VB_PRNG_H

#include <vb/common.h>

#include <boost/random.hpp>

namespace vb {
  /// Adaptor class to generate random variables from a standard PRNG.
  template <typename G> class PRNG_base : public G {
  public:
    /// Constructor, initializing if a seed is provided.
    PRNG_base (unsigned long s=0) { if (s) G::seed(s); }

    /// Generate a Bernoulli random variable of given parameter.
    double bernoulli (double p=0.5) {
      boost::bernoulli_distribution<> dist(p);
      boost::variate_generator<G&, boost::bernoulli_distribution<> > die (*this, dist);
      return die();
    }

    /// Generate an exponential random variable of given parameter.
    double exponential (double lambda=1.0) {
      boost::exponential_distribution<> dist(lambda);
      boost::variate_generator<G&, boost::exponential_distribution<> > die (*this, dist);
      return die();
    }

    /// Generate a Gaussian random variable of given parameter.
    double gaussian (double m=0.0, double sigma=1.0) {
      boost::normal_distribution<> dist(m,sigma);
      boost::variate_generator<G&, boost::normal_distribution<> > die (*this, dist);
      return die();
    }

    /// Generate a geometric random variable of given parameter.
    double geometric (double p=1.0) {
      boost::geometric_distribution<> dist(p);
      boost::variate_generator<G&, boost::geometric_distribution<> > die (*this, dist);
      return die();
    }

    /// Generate a Poisson random variable of given parameter.
    double poisson (double lambda=1.0) {
      boost::poisson_distribution<> dist(lambda);
      boost::variate_generator<G&, boost::poisson_distribution<> > die (*this, dist);
      return die();
    }

    /// Generate a uniform (continuous) random variable between the given bounds.
    double uniform_real (double min=0.0, double max=1.0) {
      boost::uniform_real<> dist(min, max);
      boost::variate_generator<G&, boost::uniform_real<> > die (*this, dist);
      return die();
    }

    /// Generate a uniform (integer) random variable between 0 and the given bound (excluded).
    unsigned int uniform_int (unsigned int range) {
      boost::uniform_int<> dist(0, range-1);
      boost::variate_generator<G&, boost::uniform_int<> > die (*this, dist);
      return die();
    }
  };

  /// Reference implementation of PRNG_base, using the standard Mersenne twister.
  typedef PRNG_base <boost::mt19937> PRNG;

  /// A global instance of PRNG to make things simpler.
  extern PRNG prng;

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

  class PRNG_Rewindable {
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

    PRNG_Rewindable (long aa=13, long bb=257, long mm=2147483647) {
      max = mm; a = aa; b = bb;
      long long  t_a=a, t_b=max, t_u=1, t_v=0, t_s=0, t_t=1;

      while (t_b != 0) {
        long long  t_q = t_a/t_b;
        long long  t_r = t_a - t_b*t_q;
        t_a = t_b; t_b = t_r;

        long long  r1 = t_u - t_q*t_s;
        t_u = t_s; t_s = r1;

        long long  r2 = t_v - t_q*t_t;
        t_v = t_t; t_t = r2;
      }

      r_a = t_u;
      r_b = (long) ( (- (long long)b * (long long)r_a ) % (long long)max );

      struct timeval tv;
      time_t curtime;

      gettimeofday (&tv, NULL);
      curtime = tv.tv_sec;
      rdmbuf = tv.tv_usec;
    }

    /** Iterate x -> a*x+b, n times.
     *
     * This is done using a dyadic algorithm, so it takes time
     * O(log(n)).
     *
     * @param a The multiplicator.
     * @param b The shift.
     * @param n The number of iterations to perform.
     */

    void iterate (long long a, long long b, long long n) {
      while (n>0) {
        if (n%2) rdmbuf = (a*rdmbuf+b) % max;
        b = ((a+1)*b) % max;
        a = (a*a) % max;
        n >>= 1;
      }
    }

    /** Rewind the renerator, time1*time2 times.
     *
     * Internally it just calls iterate(r_a,r_b,time1*time2).
     *
     * @param time1 The first factor of the number of iterations.
     * @param time2 The second factor of the number of iterations.
     */

    void rewind (long time1, long time2) {
      iterate (r_a, r_b, (long long)time1 * (long long)time2);
    }

    /// Return a pseudo-random number.
    unsigned long operator() () {
      rdmbuf = (a*rdmbuf+b)%max;
      return (unsigned long) rdmbuf;
    }

    /// The largest value that the PRNG can return.
    unsigned long max;

  private:
    long a,b, r_a,r_b;
    long long rdmbuf;
  };
}
#endif
