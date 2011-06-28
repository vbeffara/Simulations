/// @file
/// Definitions for the vb::Watcher class

#ifndef __VB_WATCHER_H
#define __VB_WATCHER_H

#include <vb/AutoWindow.h>

#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>

/** The namespace where libvb puts everything it defines. */

namespace vb {
  class Value_base {
  public:
    virtual void print_on (std::ostream &os) const =0;
    std::string name;
  };

  template <typename T> class Value : public Value_base {
  public:
    Value (const T & t_, const std::string & n = "") : t(t_) { name = n; }
    void print_on (std::ostream &os) const { os << t; }
  private:
    const T & t;
  };

  template <typename T> class Value_calc : public Value_base {
    typedef T calc_function (void*);
  public:
    Value_calc (calc_function f_, void *data, const std::string & n = "") : f(f_), d(data) { name = n; }
    void print_on (std::ostream &os) const { os << f(d); }
  private:
    calc_function *f;
    void *d;
  };

  std::ostream & operator<< (std::ostream &os, const Value_base *V);

#ifdef HAVE_FLTK
  class Watcher_slot : public Fl_Group {
  public:
    Watcher_slot (int xx, int yy, int ww, int hh, Value_base * vv);
  protected:
    void draw ();
  private:
    Value_base *v;
    Fl_Output *o;
  };
#endif

  class Watcher : public AutoWindow {
  public:
    Watcher ();
    ~Watcher ();

    void watch (Value_base *v);

    void print_on (std::ostream &os) const;
    friend std::ostream & operator<< (std::ostream &os, const Watcher &W);

  private:
    std::vector <Value_base *> l;
    int task;
  };

  void Watcher_cerr (void * W);
}
#endif
