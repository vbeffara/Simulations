
#include <vb/Watcher.h>

namespace vb {
  std::ostream & operator<< (std::ostream &os, const Value_base *V) {
    V->print_on(os);
    return os;
  }

  Watcher::Watcher () : AutoWindow (400,0,"Watcher") {
    task = global_clock.add (100, Watcher_cerr, this);
  }

  Watcher::~Watcher () {
    global_clock.remove(task);
  }

  void Watcher::add (Value_base *v) {
    l.push_back (v);
#ifdef HAVE_FLTK
    size (w(), h()+30);
    v->position (0,h()-30);
    Fl_Button * B = new Fl_Button (v->x(),v->y(),     150,30, v->name.c_str()); v->add(B);
    Fl_Output * O = new Fl_Output (v->x()+150,v->y(), 250,30);                  v->add(O);
    o.push_back(O);
#endif
  }

#ifdef HAVE_FLTK
  void Watcher::draw () {
    for (int i = 0; i < l.size(); ++i) {
      std::ostringstream os; os << l[i];
      o[i]->value(os.str().c_str());
    }
    AutoWindow::draw();
  }
#endif

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
