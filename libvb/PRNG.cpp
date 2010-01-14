
/// @file PRNG.cpp
/// Implementation of the vb::PRNG class

#include <vb/PRNG.h>

namespace vb {

  static long getseed() {
    struct timeval tv;
    time_t curtime;
    long usec;

    gettimeofday (&tv, NULL); 
    curtime = tv.tv_sec;
    usec = tv.tv_usec;

    return usec;
  }

  PRNG_base::~PRNG_base () {};

  double PRNG_base::gaussian (double m, double sigma2) {
    double modulus = exponential();
    double angle = uniform(1000.0*3.14159265358979);
    return m + sqrt(sigma2)*modulus*cos(angle);
  }

  int PRNG_base::poisson (double lambda) {
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

  PRNG_MT::PRNG_MT (unsigned long seed) {
    max = 0xffffffffUL;
    mt = new unsigned long [PRNG_MT_N];
    srand(seed);
  }

  void PRNG_MT::srand (unsigned long seed) {
    if (!seed) seed = getseed();
    mt[0] = seed & 0xffffffffUL;
    for (mti = 1; mti < PRNG_MT_N; mti++) {
      mt[mti] = (1812433253UL * (mt[mti - 1] ^ (mt[mti - 1] >> 30)) + mti);
      mt[mti] &= 0xffffffffUL;
    }
  }

  void PRNG_MT::twist () {
    unsigned long y;
    static unsigned long mag01[2] = { 0x0UL, PRNG_MT_MATRIX_A };

    if (mti >= PRNG_MT_N) {
      int kk;

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
  }

  PRNG_Rewindable::PRNG_Rewindable (long aa, long bb, long mm) {
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

      srand();
    }

  void PRNG_Rewindable::iterate (long long aa, long long bb, long long n) {
    while (n>0) {
      if (n%2) rdmbuf = (aa*rdmbuf+bb)%max;
      bb = ((aa+1)*bb)%max;
      aa = (aa*aa)%max;
      n >>= 1;
    }
  }

  void PRNG_Rewindable::srand (unsigned long seed) {
    if (!seed) seed = getseed();
    rdmbuf = seed;
  }

  void PRNG_Rewindable::rewind (long time1, long time2) {
    iterate (r_a, r_b, (long long)time1 * (long long)time2);
  }

  PRNG prng;
}
