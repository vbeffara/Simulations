/// @file
/// Definitions for the vb::Clock class

#ifndef __VB_CLOCK_H
#define __VB_CLOCK_H

#include <vb/common.h>

namespace vb {
  /// A simple callback type for functions run by vb::Clock and vb::Task.
  typedef void callback (void*);

  /// The description of a task to be run regularly by a vb::Clock.
  class Task {
  public:
    /// The constructor.
    Task (long p, callback *t, void *d) : period(p), last(-1), active(true), task(t), data(d) {}

    /// Run the task.
    void run () { task(data); }

    long period;      ///< The period (in hundredths of a second) at which we should run.
    long last;        ///< The date (in hundredths of a second) of the last run of the task.
    bool active;      ///< Whether the task should be run at all.
  private:
    callback * task;  ///< The function to be run to perform the task.
    void * data;      ///< Context passed to vb::Task::task at each run.
  };

  /// A clock that knows how to run tasks at (more or less) regular intervals.
  class Clock {
  public:
    /// The constructor.
    Clock ();

    long clock () const;  ///< Compute the elapsed time since creation, in clock ticks.
    double time () const; ///< Compute the elapsed time since creation, in seconds.
    long count () const;  ///< Compute the elapsed time since creation, in hundredths of a second.

    /// Increment the counters - this is what should be called regularly for things to run.
    void step () { ++n_call; --next; if (!next) run(); }

    /// Run all the tasks tracked by the clock, if they are ripe.
    void run ();

    /// Add a new task to be run regularly.
    int add (long period, callback * task, void *data = 0);

    /// Deactivate the given task.
    void remove (int i);

  private:
    long next;
    unsigned long long slice;
    unsigned long long n_call;
    std::vector <Task> T;
  };

  /// A global instance of vb::Clock, which should be enough for a typical program.
  extern Clock global_clock;
}

#endif
