
/** @file ProgressBar.h
 * Definitions for the vb::ProgressBar class
 */

#ifndef __VB_PROGRESS_BAR_H
#define __VB_PROGRESS_BAR_H

#include <time.h>
#include <iostream>
#include <iomanip>

namespace vb {

  /** A class to show a progress bar.
   *
   * It will output to the standard error stream.
   */

  class ProgressBar {
    public:

      /// The constructor, defaults to a length of 100 steps.

      ProgressBar (int length=100);

      /// Update the bar on the screen.

      void update (int pos);

      /// Close the display properly.

      void die (void);

    private:
      int final;            ///< The final value for the counter
      int current;          ///< The current value of the counter

      int start_time;       ///< Start at which the progress bar was created
      int last_time;        ///< Last time the ETA was computed

      int nchar;            ///< Number of displayed characters on the screen

      void display (void);  ///< Actually draw the bar on the screen
  };
 
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

#endif
