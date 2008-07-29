
/// @file ProgressBar.cpp
/// Implementation of the vb::ProgressBar class

#include <vb/ProgressBar.h>

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
    std::cerr << "\r[";
    for (int i=0; i<nchar; ++i)
      std::cerr << "=";
    for (int i=nchar; i<50; ++i)
      std::cerr << " ";
    std::cerr << "]";
    
    // std::cerr << " (" << current << "/" << final << ")";

    if (last_time > start_time) {
      double steps_per_sec = (double)current / (double)(last_time-start_time);
      int eta = (int)((double)(final-current)/steps_per_sec);

      int sec = eta%60; eta = (eta-sec)/60;    // in minutes
      int min = eta%60; eta = (eta-min)/60;    // in hours
      int hour = eta%24; eta = (eta-hour)/24;  // in days

      std::cerr << " ETA: ";
      if (eta>0)
        std::cerr << eta << "d ";
      if ((eta>0)||(hour>0))
        std::cerr << hour << "h ";
      std::cerr << min << ":" << std::setw(2) << std::setfill('0') << sec;

      std::cerr << "     ";
    }
  }
}
