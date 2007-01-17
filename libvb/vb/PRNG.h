
/// @file PRNG.h
/// Definitions for the vb::PRNG class

#ifndef __VB_PRNG_H
#define __VB_PRNG_H

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

  private:
    long a,b, r_a,r_b;
    long long rdmbuf;
  };
}

#endif
