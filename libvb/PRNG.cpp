/// @file
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

  template <typename G> int PRNG_base<G>::poisson (double lambda) {
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
    this->seed(seed);
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

      rdmbuf = getseed();
    }

  void PRNG_Rewindable::iterate (long long aa, long long bb, long long n) {
    while (n>0) {
      if (n%2) rdmbuf = (aa*rdmbuf+bb)%max;
      bb = ((aa+1)*bb)%max;
      aa = (aa*aa)%max;
      n >>= 1;
    }
  }

  void PRNG_Rewindable::rewind (long time1, long time2) {
    iterate (r_a, r_b, (long long)time1 * (long long)time2);
  }

  PRNG prng;
}
