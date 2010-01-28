
/// @file ProgressBar.cpp
/// Implementation of the vb::ProgressBar class

#include <vb/ProgressBar.h>

namespace vb {
  ProgressBar::ProgressBar (int length, double pow) : 
    final(length), current(0), nchar(0), power(pow), timer (Timer(1.0)) {
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

  void ProgressBar::display (void) {
    std::ostringstream bar;

    bar << "\r[";
    for (int i=0; i<nchar; ++i)
      bar << "=";
    for (int i=nchar; i<50; ++i)
      bar << " ";
    bar << "]";
    
    // std::cerr << " (" << current << "/" << final << ")";

    if (timer.t_elapsed() > 0) {
      double done = pow(current,power);
      double todo = pow(final,power);
      int eta = int (timer.t_elapsed() * (todo/done - 1.0));

      bar << " ETA: ";

      int tmp;
      if ((tmp = eta/3600/24)) { bar << tmp << "d "; eta -= tmp*3600*24; }
      if ((tmp = eta/3600))    { bar << tmp << "h "; eta -= tmp*3600; }
      if ((tmp = eta/60))      { bar << tmp << "m "; eta -= tmp*60; }
      bar << eta << "s     ";
    }

    std::cerr << bar.str();
  }
}
