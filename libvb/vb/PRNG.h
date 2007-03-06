
/// @file PRNG.h
/// Definitions for the vb::PRNG class

#ifndef __VB_PRNG_H
#define __VB_PRNG_H

#include <math.h>
#include <time.h>

namespace vb {

  /** A rewindable pseudo-random number generator engine.
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

  class PRNG_Engine_Rewindable {
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

      PRNG_Engine_Rewindable (long aa=13, long bb=257, long mm=2147483647);

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

  class PRNG_Engine_MT {
    public:

      /** The maximum integer that this PRNG can return. */

      static const unsigned long max = 0xffffffffUL;
      
      /** The standard constructor. */

      PRNG_Engine_MT () { mti = PRNG_MT_N+1; }

      /** Initialize the PRNG from an integer seed. */

      void srand (unsigned long seed) {
        mt[0] = seed & 0xffffffffUL;
        for (mti = 1; mti < PRNG_MT_N; mti++) {
          mt[mti] = (1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
          mt[mti] &= 0xffffffffUL;
        }
      }

      /** Return an integer between 0 and max. */

      unsigned long rand () {
        unsigned long y;
        static unsigned long mag01[2] = { 0x0UL, PRNG_MT_MATRIX_A };

        if (mti >= PRNG_MT_N) {
          int kk;

          if (mti == PRNG_MT_N + 1) srand (5489UL);

          for (kk = 0; kk < PRNG_MT_N - PRNG_MT_M; kk++) {
            y = (mt[kk] & PRNG_MT_UPPER_MASK) | (mt[kk + 1] & PRNG_MT_LOWER_MASK);
            mt[kk] = mt[kk + PRNG_MT_M] ^ (y >> 1) ^ mag01[y & 0x1UL];
          }

          for (; kk < PRNG_MT_N - 1; kk++) {
            y = (mt[kk] & PRNG_MT_UPPER_MASK) | (mt[kk + 1] & PRNG_MT_LOWER_MASK);
            mt[kk] = mt[kk + (PRNG_MT_M - PRNG_MT_N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
          }

          y = (mt[PRNG_MT_N - 1] & PRNG_MT_UPPER_MASK) | (mt[0] & PRNG_MT_LOWER_MASK);
          mt[PRNG_MT_N - 1] = mt[PRNG_MT_M - 1] ^ (y >> 1) ^ mag01[y & 0x1UL];

          mti = 0;
        }

        y = mt[mti++];

        /* Tempering */

        y ^= (y >> 11);
        y ^= (y << 7) & 0x9d2c5680UL;
        y ^= (y << 15) & 0xefc60000UL;
        y ^= (y >> 18);

        return y;
      }

    private:
      unsigned long mt[PRNG_MT_N];
      int mti;
  };

  /** A general-purpose pseudo-random number generator template.
   *
   * It encapsulates one of the PRNG_Engine_* engines - though you can
   * use your own class, all it needs is one unsigned long member called
   * max and two methods,
   *
   * - unsigned long rand () 
   * - void srand (unsigned long)
   *
   * where rand() is expected to return a number between 0 and max.
   *
   * Several classical distributions are implemented, see the
   * documentation for a complete list (I add them as I need them ...)
   */

  template <class Engine> class PRNG_template : public Engine {
    public:

      /** The standard constructor, initializes using time(0). */

      PRNG_template () { srand(time(0)); }

      /** A constructor taking a random seed to initialize the engine. */

      PRNG_template (long seed) { srand(seed); }

      /** Return a bernoulli variable in {0,1} */

      int bernoulli (double p=0.5) {
        return rand() < p*(double)max ? 1 : 0;
      }

      /** Return a uniformly distributed real between 0 and range */

      double uniform (double range=1.0) {
        return range * ( (double)rand() / (double)max );
      }

      /** Return an exponential random variable of parameter lambda */

      double exponential (double lambda=1.0) {
        return -log(uniform())/lambda;
      }

      /** Return a Gaussian variable.
       * Caution : it calls rand() twice, if you rewind you should know it.
       */

      double gaussian (double m=0.0, double sigma2=1.0) {
        double modulus = exponential();
        double angle = uniform(1000.0*3.14159265358979);
        return m + sqrt(sigma2)*modulus*cos(angle);
      }

      /** Return a geometric variable (in Z_+) of parameter p. */

      int geometric (double p) {
        return (int) floor(exponential(-log(1-p)));
      }

      /** Return a Poisson variable of parameter lambda. */

      int poisson (double lambda) {
        double u = uniform(exp(lambda));
        int k=0;
        double fk=1;
        while (u>0) {
          u -= pow(lambda,k)/fk;
          ++k;
          fk *= k;
        }
        return k-1;
      }
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

  typedef PRNG_template<PRNG_Engine_MT> PRNG;

  /** The rewindable pseudo-random number generator.
   *
   * It uses the 'rewindable' engine (which is a terribly bad linear
   * congruence iterator), and implements everything that PRNG_template
   * provides.
   *
   * To rewind, use PRGN_Rewindable::rewind(...).
   */

  typedef PRNG_template<PRNG_Engine_Rewindable> PRNG_Rewindable;
}

#endif
