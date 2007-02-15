
#include <vb/PRNG.h>

namespace vb {

  PRNG_Engine_Rewindable::PRNG_Engine_Rewindable (long aa, long bb, long mm) :
    max(mm), a(aa), b(bb) {
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
  }

  void PRNG_Engine_Rewindable::iterate (long long aa, long long bb, long long n) {
    while (n>0) {
      if (n%2) rdmbuf = (aa*rdmbuf+bb)%max;
      bb = ((aa+1)*bb)%max;
      aa = (aa*aa)%max;
      n >>= 1;
    }
  }

  void PRNG_Engine_Rewindable::srand (long seed) {
    rdmbuf = seed;
  }

  void PRNG_Engine_Rewindable::rewind (long time1, long time2) {
    iterate (r_a, r_b, (long long)time1 * (long long)time2);
  }

}
