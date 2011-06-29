/// @file
/// Definitions for the vb::Clock class

#ifndef __VB_CLOCK_H
#define __VB_CLOCK_H

#include <vb/common.h>

namespace vb {
  typedef void callback (void*);

  class Task {
  public:
    Task (long p, callback *t, void *d) : period(p), last(-1), active(true), task(t), data(d) {}
    void run () { task(data); }
    long period;
    long last;
    bool active;
  private:
    callback * task;
    void * data;
  };

  class Clock {
  public:
    Clock ();

    long clock () const;
    double time () const;
    long count () const;

    void step () { ++n_call; --next; if (!next) run(); }
    void run ();

    int add (long period, callback * task, void *data = 0);
    void remove (int i);

  private:
    long next;
    unsigned long long slice;
    unsigned long long n_call;
    std::vector <Task> T;
  };

  extern Clock global_clock;
}

#endif
