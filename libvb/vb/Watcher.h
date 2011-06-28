/// @file
/// Definitions for the vb::Watcher class

#ifndef __VB_WATCHER_H
#define __VB_WATCHER_H

#include <vb/AutoWindow.h>

#include <FL/Fl_Button.H>
#include <FL/Fl_Output.H>

/** The namespace where libvb puts everything it defines. */

namespace vb {
  class Value_base : public Fl_Group {
  public:
    Value_base () : Fl_Group (0,0, 400,30, "") {};
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

  class Watcher : public AutoWindow {
  public:
    Watcher ();
    ~Watcher ();

    void add (Value_base *v);

    void print_on (std::ostream &os) const;
    friend std::ostream & operator<< (std::ostream &os, const Watcher &W);

#ifdef HAVE_FLTK
  protected:
    void draw ();
#endif

  private:
    std::vector <Value_base *> l;
    std::vector <Fl_Output *> o;
    int task;
  };

  void Watcher_cerr (void * W);
}
#endif
