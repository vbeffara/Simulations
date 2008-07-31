
/// @file ProgressBar.cpp
/// Implementation of the vb::ProgressBar class

#include <vb/ProgressBar.h>
#include <sstream>

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
    update(final);
    std::cerr << "\n";
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

    if (last_time > start_time) {
      double steps_per_sec = (double)current / (double)(last_time-start_time);
      int eta = (int)((double)(final-current)/steps_per_sec);

      bar << " ETA: ";

      if (eta < 60) bar << eta << " second(s)"; else {
        eta /= 60;
        if (eta < 60) bar << eta << " minute(s)"; else {
          eta /= 60;
          if (eta < 24) bar << eta << " hour(s)"; else {
            eta /= 24;
            if (eta < 365) bar << eta << " day(s)"; else {
              eta /= 365;
              bar << eta << " year(s)";
            }
          }
        }
      }

      bar << "     ";
      std::cerr << bar.str();
    }
  }
}
