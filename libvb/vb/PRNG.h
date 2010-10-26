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

  class PRNG_base {
    public:
      unsigned long max; ///< The range of the rand() method.

      virtual ~PRNG_base (); ///< The standard destructor.

      /** Initialize the PRNG from a seed.
       *
       * @param seed The initialization seed.
       */

      virtual void srand (unsigned long seed) =0;

      /** Produce a pseudo-random number generator.
       *
       * If possible, the implementation should be mostly inlined.
       */

      virtual unsigned long rand (void) =0;

      /** Return a bernoulli variable in {0,1}
       *
       * @param p The parameter of the distribution.
       */

      int bernoulli (double p) {
        return rand() < p*(double)max ? 1 : 0;
      }

      /** Return a uniformly distributed real between 0 and range
       *
       * @param range The length of the range.
       */

      double uniform (double range=1.0) {
        return range * ( (double)rand() / (double)max );
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
       * Caution : it calls rand() twice, if you rewind you should know it.
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

  class PRNG_Rewindable : public PRNG_base {
    public:
      /** The standard constructor.
       *
       * Default values of the parameters are the same as in standard Unix
       * rand(), i.e. a=13, b=257, modulo 2**31-1.
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

      unsigned long rand (void) {
        rdmbuf = (a*rdmbuf+b)%max;
        return (unsigned long) rdmbuf;
      }

    private:
      long a,b, r_a,r_b;
      long long rdmbuf;
  };

#define PRNG_MT_N 624                   ///< Size of the buffer
#define PRNG_MT_M 397                   ///< Size of the shift (?)
#define PRNG_MT_MATRIX_A 0x9908b0dfUL	///< Constant vector a
#define PRNG_MT_UPPER_MASK 0x80000000UL	///< Most significant w-r bits
#define PRNG_MT_LOWER_MASK 0x7fffffffUL	///< Least significant r bits

  /** Mersenne Twister pseudo-random number generator engine.
   *
   * Original version : mt19937ar.c by Takuji Nishimura and Makoto
   * Matsumoto - see original copyright notice below.
   *
   * Translated to C++ by Vincent Beffara - I also removed the
   * initialization by an array (at least for now).
   *
   * ORIGINAL COPYRIGHT NOTICE FROM mt19937ar.c:
   *
   * A C-program for MT19937, with initialization improved 2002/1/26.
   * Coded by Takuji Nishimura and Makoto Matsumoto.
   *
   * Before using, initialize the state by using init_genrand(seed)  
   * or init_by_array(init_key, key_length).
   *
   * Copyright (C) 1997 - 2002, Makoto Matsumoto and Takuji Nishimura,
   * All rights reserved.                          
   *
   * Redistribution and use in source and binary forms, with or without
   * modification, are permitted provided that the following conditions
   * are met:
   *
   * 1. Redistributions of source code must retain the above copyright
   * notice, this list of conditions and the following disclaimer.
   *
   * 2. Redistributions in binary form must reproduce the above copyright
   * notice, this list of conditions and the following disclaimer in the
   * documentation and/or other materials provided with the distribution.
   *
   * 3. The names of its contributors may not be used to endorse or promote 
   * products derived from this software without specific prior written 
   * permission.
   *
   * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
   * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
   * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   * A PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT
   * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
   * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
   * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
   * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
   * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
   *
   * Any feedback is very welcome.
   *     http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/emt.html
   *     email: m-mat @ math.sci.hiroshima-u.ac.jp (remove space)
   */

  class PRNG_MT : public PRNG_base {
    public:
      PRNG_MT (unsigned long seed = 0); ///< The standard constructor.

      /** Initialize the PRNG from an integer seed.
       * 
       * @param seed The initialization seed.
       */

      void srand (unsigned long seed = 0);

      /// Return an integer between 0 and max.

      unsigned long rand () { return gen(); }

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
}

#endif
