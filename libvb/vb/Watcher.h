/// @file
/// Definitions for the vb::Watcher class

#pragma once
#include <vb/AutoWindow.h>

/** The namespace where libvb puts everything it defines. */

namespace vb {
  /** An abstract base class for holding a value.
   *
   * This is a way of enabling vb::Watcher to track a value of an
   * arbitrary type using a template. All that is needed is the ability
   * to output to a stream.
   */

  class Value_base {
  public:
    /// Output the hold value to a stream.
    virtual void print_on (std::ostream &os) const =0;

    /// The name (label) of the hold value.
    std::string name;
  };

  /// A subclass of vb::Value_base holding a reference to an outside variable.

  template <typename T> class Value : public Value_base {
  public:
    /// Constructor.
    Value (const T & t_, const std::string & n = "") : t(t_) { name = n; }
    void print_on (std::ostream &os) const { os << t; }
  private:
    const T & t;
  };

  /** A subclass of vb::Value_base for a computed value.
   *
   * Each instance holds a function, and asking for the hold value
   * returns the output of that function.
   */

  template <typename T> class Value_calc : public Value_base {
    typedef T calc_function (void*);
  public:
    /// Constructor.
    Value_calc (calc_function f_, void *data, const std::string & n = "") : f(f_), d(data) { name = n; }

    /// Output the output of vb::Value_calc::f to a stream.
    void print_on (std::ostream &os) const { os << f(d); }
  private:
    calc_function *f;
    void *d;
  };

  /// Output the hold value to a stream.
  std::ostream & operator<< (std::ostream &os, const Value_base *V);

  class Watcher_slot : public Fl_Group {
  public:
    Watcher_slot (int ww, Value_base * vv);
  protected:
    void draw ();
  private:
    Value_base *v;
    Fl_Output *o;
  };

  /** A utility class to observe a running simulation.
   *
   * Add variables to track, display using show() and the contents of
   * the variables will self-update on screen automatically.
   */

  class Watcher : public AutoWindow {
  public:
    Watcher (bool e = false);
    ~Watcher ();

    /// Add a new thing to watch.
    void watch (Value_base *v);

    /// Shortcut for simple values.
    template <typename T> void watch (const T & t, const std::string & n) {
      watch (new Value<T> (t,n));
    }

    /// Output all watched things to a stream.
    void print_on (std::ostream &os) const;

    /// Output all watched things to a stream.
    friend std::ostream & operator<< (std::ostream &os, const Watcher &W);

  private:
    std::vector <Value_base *> l;
    int task;
  };

  /// Output the watched variables od a vb::Watcher to std::cerr.
  void Watcher_cerr (void * W);
}
