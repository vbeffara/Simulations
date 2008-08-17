/// @file ProgressBar.h
/// Definitions for the vb::ProgressBar class

#ifndef __VB_PROGRESS_BAR_H
#define __VB_PROGRESS_BAR_H

#include <vb/Timer.h>

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

      int nchar;            ///< Number of displayed characters on the screen

      Timer timer;          ///< Timer for automatic updating

      void display (void);  ///< Actually draw the bar on the screen
  };
}

#endif
