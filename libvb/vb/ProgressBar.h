/// @file
/// Definitions for the vb::ProgressBar class

#pragma once
#include <vb/AutoWindow.h>

namespace vb {

  /** A class to show a progress bar.
   *
   * It will output to the standard error stream.
   */

  class ProgressBar : public AutoWindow {
  public:
    /// The constructor, defaults to a length of 100 steps.
    ProgressBar (int length=100, double pow=1.0);

    /// The destructor, which closes the display properly.
    ~ProgressBar ();

    /// Update the bar on the screen.
    void set (int pos);

  private:
    void update();        ///< Display the bar on screen.

    int final;            ///< The final value for the counter.
    int current;          ///< The current value of the counter.

    int nchar;            ///< Number of displayed characters on the screen.

    double power;         ///< Power law driving the progress.
  };
}
