
#include <vb/Console.h>

namespace vb {
  std::ostream & operator<< (std::ostream &os, const Value_base *V) {
    V->print_on(os);
    return os;
  }

  Console_slot::Console_slot (int ww, Value_base * vv) :
    Fl_Group (0,0,ww,30), v(vv) {
    new Fl_Button (x(),y(), 150,h(), vv->name.c_str());
    o = new Fl_Output (x()+150,y(), w()-150,h());
  }

  void Console_slot::draw () {
    std::ostringstream os; os << v;
    o->value(os.str().c_str());
    Fl_Group::draw();
  }

  Console::Console (bool e) : AutoWindow (400,0,"Console") {
    if (e) task = global_clock.add (100, Console_cerr, this);
  }

  Console::~Console () {
    global_clock.remove(task);
  }

  void Console::watch (Value_base *v) {
    l.push_back (v);
    Console_slot * W = new Console_slot (w(),v);
    W -> position (0,h());
    size (w(), h()+W->h());
    add (W);
  }

  std::ostream & operator<< (std::ostream &os, const Console &W) {
    for (int i = 0; i < W.l.size(); ++i) {
      os << " | ";
      if (W.l[i]->name != "") os << W.l[i]->name << " = ";
      os << W.l[i];
    }
    return os << " |" << std::endl;
  }

  void Console_cerr (void * W) { std::cerr << *((Console*)W); }
}
