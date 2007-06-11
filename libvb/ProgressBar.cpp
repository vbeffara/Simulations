
#include <vb/ProgressBar.h>
#include <time.h>
#include <iostream>

namespace vb {

  ProgressBar::ProgressBar (int length) : 
    final(length), current(0), start_time(time(0)), last_time(time(0)), nchar(0) {
      display();
    }

  void ProgressBar::update (int pos) {
    if (pos<0) pos=0;
    if (pos>final) pos=final;

    current = pos;
    int dirty = 0;
    int new_time = time(0);
    int new_nchar = (pos*50)/final;

    if (new_nchar > nchar) {
      nchar = new_nchar;
      last_time = new_time + 1;
      dirty = 1;
    }

    if (new_time > last_time) {
      last_time = new_time;
      dirty = 1;
    }

    if (dirty) display();
  }

  void ProgressBar::die (void) {
    std::cerr << "\n";
  }

  void ProgressBar::display (void) {
    std::cerr << "\r[";
    for (int i=0; i<nchar; ++i)
      std::cerr << "=";
    for (int i=nchar; i<50; ++i)
      std::cerr << " ";
    std::cerr << "] (" << current << "/" << final << ")";
  }
}
