
/// @file ProgressBar.cpp
/// Implementation of the vb::ProgressBar class

#include <vb/ProgressBar.h>

namespace vb {
  ProgressBar::ProgressBar (int length, double pow) : 
    final(length), current(0), nchar(0), power(pow), timer(1.0) {
      display();
    }

  ProgressBar::~ProgressBar () {
    update(final);
    std::cerr << "\n";
  }

  void ProgressBar::update (int pos) {
    if (pos<0) pos=0;
    if (pos>final) pos=final;

    current = pos;
    int dirty = 0;
    int new_nchar = (pos*50.0)/final;

    if (timer.check()) {
      timer.reset();
      dirty = 1;
    }

    if (new_nchar > nchar) {
      nchar = new_nchar;
      timer.reset();
      dirty = 1;
    }

    if (dirty) display();
  }

  void ProgressBar::display () {
    std::ostringstream bar;

    bar << "\r[";
    for (int i=0; i<nchar; ++i)  bar << "=";
    for (int i=nchar; i<50; ++i) bar << " ";
    bar << "]";
    
    if (timer.t_elapsed() > 0) {
      double done = pow(current,power), todo = pow(final,power);
      int eta = timer.t_elapsed() * (todo/done - 1.0);
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
}
