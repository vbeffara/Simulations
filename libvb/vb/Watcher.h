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
    Value (T &t_, const std::string &n = "") : t(t_) { name = n; }
    virtual void print_on (std::ostream &os) const {
      os << t;
    }
  private:
    T &t;
  };

  std::ostream & operator<< (std::ostream &os, const Value_base *V) {
    V->print_on(os);
    return os;
  }

  void Watcher_show (void *);
  void Watcher_update (void *);

  class Watcher : public Fl_Double_Window {
  public:
    Watcher () : Fl_Double_Window (400,0) { task = global_clock.add (100, Watcher_show, this); }
    ~Watcher () { global_clock.remove(task); }

    void add (Value_base *v) { l.push_back (v); }

    void print_on (std::ostream &os) const {
      for (int i = 0; i < l.size(); ++i) {
        os << " | ";
        if (l[i]->name != "") os << l[i]->name << " = ";
        os << l[i];
      }
      os << " |" << std::endl;
    }

    void show () {
      int n = l.size();
      size (400,30*n);
      begin();
      for (int i = 0; i < n; ++i) {
        new Fl_Button (0,30*i,   150,30, l[i]->name.c_str());
        o.push_back (new Fl_Output (150,30*i, 250,30));
        std::ostringstream os; os << l[i];
        o[i]->value(os.str().c_str());
      }
      end();
      Fl_Double_Window::show();
      global_clock.add (5, Watcher_update, this);
    }

    void update() {
      for (int i = 0; i < l.size(); ++i) {
        std::ostringstream os; os << l[i];
        o[i]->value(os.str().c_str());
      }
      redraw();
      Fl::check();
    }

  private:
    std::vector <Value_base *> l;
    std::vector <Fl_Output *> o;
    int task;
  };

  std::ostream & operator<< (std::ostream &os, const Watcher &W) { W.print_on(os); return os; }

  void Watcher_show (void * W) { std::cerr << *((Watcher*)W); }
  void Watcher_update (void * W) { ((Watcher*)W) -> update(); }
}

#endif
