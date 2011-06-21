/// @file
/// Definitions for the vb::Clock class

#ifndef __VB_CLOCK_H
#define __VB_CLOCK_H

#include <vb/common.h>

namespace vb {
  typedef void callback (void*);

  class Task {
  public:
    Task (long p, callback *t, void *d) : period(p), task(t), data(d) {}
    long period;
    long last;
    callback * task;
    void * data;
  };

  /** A class for a programmable clock, to eventually replace Timer().
   *
   * Planned main feature : callbacks every programmable time.
   */

  class Clock {
  public:
    long clock ()  { return std::clock(); }
    double time () { return double(clock()) / CLOCKS_PER_SEC; }
    long count ()  { return time() * 100; }

    void run () {
      for (std::list<Task>::iterator i = T.begin(); i != T.end(); ++i) {
        std::cerr << count() << "\r";
        if (count() >= i->last + i->period) {
          (i->task)(i->data);
          i->last = count();
        }
      }
    }

    void add (long period, callback * task, void *data = 0) {
      T.push_back (Task(period,task,data));
    }

  private:
    std::list <Task> T;
  };
}

#endif
