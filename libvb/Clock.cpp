
#include <vb/Clock.h>

namespace vb {
  /** A class for a programmable clock, to eventually replace Timer().
   *
   * Planned main feature : callbacks every programmable time.
   */

  Clock::Clock () : next(1), slice(10), n_call(0) {}

  long Clock::clock () const {
    return std::clock();
  }

  double Clock::time () const {
    return double(clock()) / CLOCKS_PER_SEC;
  }

  long Clock::count () const {
    return time() * 100;
  }

  void Clock::run () {
    for (std::vector<Task>::iterator i = T.begin(); i != T.end(); ++i) {
      if ((i->active) && (count() >= i->last + i->period)) {
        i->run();
        i->last = count();
      }
    }

    if (count() <= 100) slice += slice/10;
    else slice = std::min (n_call / count(), slice + slice/10);

    next = slice;
  }

  int Clock::add (long period, void task (void*), void *data) {
    T.push_back (Task(period,task,data));
    return T.size()-1;
  }

  void Clock::remove (int i) { if (i>=0) T[i].active = false; }
}
