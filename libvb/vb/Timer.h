
/// @file Timer.h
/// Definitions for the vb::Timer class

#ifndef __VB_TIMER_H
#define __VB_TIMER_H

#include <vb/common.h>

namespace vb {

  /** This is a helper class implementing a customizable timer.
   *
   * The only parameter is the period; the timer will allow for testing 
   * whether this time has elapsed, and keep track of time spent since 
   * the start of the timer and since the last reset.
   */

  class Timer {
    public:
      int sec;    ///< Elapsed seconds since the last reset
      int usec;   ///< Elapsed microseconds since the last reset
      int t_sec;  ///< Elapsed seconds since the start
      int t_usec; ///< Elapsed microseconds sonce the start

      double period;  ///< Characteristic period of the timer (in seconds)

      /** The standard creator of a vb::Timer object.
       *
       * It creates an instance of vb::Timer with the prescribed 
       * characteristic period (expressed in seconds as a double).
       *
       * @param p The characteristic period (in seconds).
       */

      Timer (double p);

      /** Check whether the characteristic period is elapsed.
       *
       * This returns true if and only if the time elapsed since the 
       * last call to reset() is larger than the characteristic period.
       *
       * This is the only method (besides the constructor) that will 
       * actually call gettimeofday(); it will also call 
       * compute(), so after the call the state of the timer is 
       * coherent. That means that the overhead can be significant 
       * (roughly one microsecond); avoid using it in a tight loop!
       */

      bool check ();

      /** Reset the timer.
       *
       * That means that the elapsed time since the last reset is set to 
       * 0. It will ensure that the timer state after the call is 
       * coherent.
       */

      void reset ();

      double elapsed ();   ///< Returns the elapsed time (in seconds) since the last reset.
      double t_elapsed (); ///< Returns the elapsed time (in seconds) since the start of the timer.

    private:
      timeval start;       ///< The starting time of the timer.
      timeval last;        ///< The time of the last reset of the timer.
      timeval current;     ///< The time at the last call of check().

      void compute ();     ///< Compute the values of sec, usec, t_sec and t_usec
  };
}

#endif
