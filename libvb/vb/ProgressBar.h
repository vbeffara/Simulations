
/** @file ProgressBar.h
 * Definitions for the vb::ProgressBar class
 */

#ifndef __VB_PROGRESS_BAR_H
#define __VB_PROGRESS_BAR_H

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
 
}

#endif
