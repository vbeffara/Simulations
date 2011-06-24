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
    virtual void print_on (std::ostream &os) const {
      os << t;
    }
  private:
    const T & t;
  };

  std::ostream & operator<< (std::ostream &os, const Value_base *V) {
    V->print_on(os);
    return os;
  }

  void Watcher_show (void *);

  class Watcher : public AutoWindow {
  public:
    Watcher () : AutoWindow (400,0,"Watcher") {
      task = global_clock.add (100, Watcher_show, this);
    }

    ~Watcher () {
      global_clock.remove(task);
    }

    void add (Value_base *v) {
      l.push_back (v);
      size (w(), h()+30);
      begin();
      new Fl_Button (0,h()-30, 150,30, v->name.c_str());
      o.push_back (new Fl_Output (150,h()-30, 250,30));
      end();
    }

    void print_on (std::ostream &os) const {
      for (int i = 0; i < l.size(); ++i) {
        os << " | ";
        if (l[i]->name != "") os << l[i]->name << " = ";
        os << l[i];
      }
      os << " |" << std::endl;
    }

    void draw () {
      for (int i = 0; i < l.size(); ++i) {
        std::ostringstream os; os << l[i];
        o[i]->value(os.str().c_str());
      }
      AutoWindow::draw();
    }

  private:
    std::vector <Value_base *> l;
    std::vector <Fl_Output *> o;
    int task;
  };

  std::ostream & operator<< (std::ostream &os, const Watcher &W) { W.print_on(os); return os; }

  void Watcher_show (void * W) { std::cerr << *((Watcher*)W); }
}

#endif
