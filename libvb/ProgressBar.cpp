/// @file
/// Implementation of the vb::ProgressBar class

#include <vb/ProgressBar.h>

namespace vb {
  ProgressBar::ProgressBar (int length, double pow) :
    final(length), current(0), nchar(0), power(pow) {
      display();
      task = global_clock.add (10, ProgressBar_display, this);
    }

  ProgressBar::~ProgressBar () {
    update(final);
    display();
    std::cerr << "\n";
    global_clock.remove(task);
  }

  void ProgressBar::update (int pos) {
    if (pos<0) pos=0;
    if (pos>final) pos=final;
    current = pos;
    nchar = (pos*50.0)/final;
    global_clock.step();
  }

  void ProgressBar::display () {
    std::ostringstream bar;

    bar << "\r[";
    for (int i=0; i<nchar; ++i)  bar << "=";
    for (int i=nchar; i<50; ++i) bar << " ";
    bar << "]";

    if (global_clock.time() > 3) {
      double done = pow(current,power), todo = pow(final,power);
      int eta = global_clock.time() * (todo/done - 1.0);
      bool big=false;

      bar << " (";
      if (eta >= 3600*24) {
        bar << eta/3600/24 << "d ";
        eta = eta % 3600*24;
        big = true;
      }
      if (big || (eta >= 3600)) {
        bar << eta/3600 << ":" << std::setw(2) << std::setfill('0');
        eta = eta % 3600;
      }
      bar << eta/60 << ":" << std::setw(2) << std::setfill('0') << eta%60 << ")   ";
    }

    std::cerr << bar.str();
  }

  void ProgressBar_display (void * PB) { ((ProgressBar*)PB) -> display(); }
}
