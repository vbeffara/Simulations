
/// @file Timer.h
/// Definitions for the vb::Timer class

#ifndef __VB_TIMER_H
#define __VB_TIMER_H

#include <sys/time.h>

namespace vb {
  class Timer {
    public:
      int sec, usec, t_sec, t_usec;
      int period;

      Timer (int p);

      void compute ();
      bool check ();
      void reset ();

    private:
      timeval start;
      timeval last;
      timeval current;
  };
}

#endif
