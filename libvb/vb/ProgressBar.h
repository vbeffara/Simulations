/// @file
/// Definitions for the vb::ProgressBar class

#ifndef __VB_PROGRESS_BAR_H
#define __VB_PROGRESS_BAR_H

#include <vb/Clock.h>

namespace vb {

  /** A class to show a progress bar.
   *
   * It will output to the standard error stream.
   */

  class ProgressBar {
    public:
      /// The constructor, defaults to a length of 100 steps.
      ProgressBar (int length=100, double pow=1.0);

      /// The destructor, which closes the display properly.
      ~ProgressBar ();

      /// Update the bar on the screen.
      void update (int pos);

      void display (void);  ///< Actually draw the bar on the screen.

    private:
      int final;            ///< The final value for the counter.
      int current;          ///< The current value of the counter.

      int nchar;            ///< Number of displayed characters on the screen.

      double power;         ///< Power law driving the progress.

      int task;
  };

  void ProgressBar_display (void *);
}

#endif
