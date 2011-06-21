/// @file
/// Definitions for the vb::Watcher class

#ifndef __VB_WATCHER_H
#define __VB_WATCHER_H

#include <vb/Globals.h>

/** The namespace where libvb puts everything it defines. */

namespace vb {
  class Value_base {
  public:
    virtual void print_on (std::ostream &os) const =0;
  };

  template <typename T> class Value : public Value_base {
  public:
    Value (T &t_, const std::string &n = "") : name(n), t(t_) { }
    virtual void print_on (std::ostream &os) const {
      if (name != "") os << name << " = ";
      os << t;
    }
  private:
    std::string name;
    T &t;
  };

  std::ostream & operator<< (std::ostream &os, const Value_base *V) {
    V->print_on(os);
    return os;
  }

  class Watcher {
  public:
    void add (Value_base *v) { l.push_back (v); }

    void print_on (std::ostream &os) const {
      for (std::list<Value_base*>::const_iterator i = l.begin(); i != l.end(); ++i)
        os << " | " << *i;
      os << " |" << std::endl;
    }

  private:
    std::list <Value_base *> l;
  };

  std::ostream & operator<< (std::ostream &os, const Watcher &W) { W.print_on(os); return os; }
}

#endif
