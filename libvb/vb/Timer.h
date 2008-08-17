
/// @file Timer.h
/// Definitions for the vb::Timer class

#ifndef __VB_TIMER_H
#define __VB_TIMER_H

#include <sys/time.h>

namespace vb {
  class Timer {
    public:
      int sec, usec, t_sec, t_usec;
      double period;

      Timer (double p);

      void compute ();
      bool check ();
      void reset ();

      double elapsed ();
      double t_elapsed ();

    private:
      timeval start;
      timeval last;
      timeval current;
  };
}

#endif
