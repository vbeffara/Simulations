/// @file
/// Definitions for the vb::AutoWindow class.

#pragma once
#include <vb/Clock.h>

namespace vb {
  /** A nice helper class for simulations.
   *
   * The AutoWindow class takes care of displaying the window on the
   * screen, catching keypresses, and changing the display approximately
   * 25 times per second.  A derived class is expected to do two things:
   *
   * (i) provide a paint() method to fill in AutoWindow::surface
   * (posibly making use of AutoWindow::cr if needed). Updating it
   * elsewhere in the code is safe, because paint() will always be
   * called just before surface() is used;
   *
   * (ii) call AutoWindow::step() often enough, so that the display is
   * updated with the correct frequency. But this is not mandatory.
   */

  class AutoWindow : public Fl_Double_Window {
  public:
    std::string title;       ///< The title of the window.

    /** The standard constructor
     *
     * @param wd The width of the window.
     * @param ht The height of the window.
     * @param t  The title of the window.
     */

    AutoWindow (int wd, int ht, const std::string &t);
    ~AutoWindow ();

    /// Show the window on the screen.
    void show ();

    /// Update the screen, handle the events.
    virtual void update ();

    /// Put the image on pause, i.e. wait for user input.
    void pause() { paused=true; update(); }

    /// Increment the clock and call cycle() as needed.
    void step() { global_clock.step(); }

    virtual int handle (int event);   ///< Handle the events, in particular 'q' and 'x'.

  private:
    bool paused;
    int task;
  };

  /// Re-draw the vb::AutoWindow passed as parameter.
  void AutoWindow_update (void * AW);
}
