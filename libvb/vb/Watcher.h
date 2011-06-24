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
    virtual void print_on (std::ostream &os) const { os << t; }
  private:
    const T & t;
  };

  std::ostream & operator<< (std::ostream &os, const Value_base *V);

  class Watcher : public AutoWindow {
  public:
    Watcher ();
    ~Watcher ();

    void add (Value_base *v);

    void print_on (std::ostream &os) const;
    friend std::ostream & operator<< (std::ostream &os, const Watcher &W);

  protected:
    void draw ();

  private:
    std::vector <Value_base *> l;
    std::vector <Fl_Output *> o;
    int task;
  };

  void Watcher_cerr (void * W);
}
#endif
