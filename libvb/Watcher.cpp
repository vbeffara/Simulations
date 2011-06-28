
#include <vb/Watcher.h>

namespace vb {
  std::ostream & operator<< (std::ostream &os, const Value_base *V) {
    V->print_on(os);
    return os;
  }

#ifdef HAVE_FLTK
  Watcher_slot::Watcher_slot (int xx, int yy, int ww, int hh, Value_base * vv) :
    Fl_Group (xx,yy,ww,hh), v(vv) {
    new Fl_Button (x(),y(), 150,30, vv->name.c_str());
    o = new Fl_Output (x()+150,y(), 250,30);
  }

  void Watcher_slot::draw () {
    std::ostringstream os; os << v;
    o->value(os.str().c_str());
    Fl_Group::draw();
  }
#endif

  Watcher::Watcher () : AutoWindow (400,0,"Watcher") {
    task = global_clock.add (100, Watcher_cerr, this);
  }

  Watcher::~Watcher () {
    global_clock.remove(task);
  }

  void Watcher::watch (Value_base *v) {
    l.push_back (v);
#ifdef HAVE_FLTK
    size (w(), h()+30);
    add (new Watcher_slot (0,h()-30, 400,30, v));
#endif
  }

  std::ostream & operator<< (std::ostream &os, const Watcher &W) {
    for (int i = 0; i < W.l.size(); ++i) {
      os << " | ";
      if (W.l[i]->name != "") os << W.l[i]->name << " = ";
      os << W.l[i];
    }
    return os << " |" << std::endl;
  }

  void Watcher_cerr (void * W) { std::cerr << *((Watcher*)W); }
}
