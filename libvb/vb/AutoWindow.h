/// @file
/// Definitions for the vb::AutoWindow class.

#ifndef AUTOWINDOW_H
#define AUTOWINDOW_H

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

  class AutoWindow
#ifdef HAVE_FLTK
    : public Fl_Double_Window
#endif
  {
  public:
    std::string title;       ///< The title of the window.
    int fps;                 ///< The target FPS rate.

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

  private:
    bool paused;
    int task;

#ifdef HAVE_FLTK
    int handle (int event);           ///< Handle the events, in particular 'q' and 'x'.
#else
    int width;                        ///< The width of the image, in pixels.
    int height;                       ///< The height of the image, in pixels.
  public:
    int w() const { return width; }   ///< Return the current width of the window.
    int h() const { return height; }  ///< Return the current height of the window.
    void size (int w, int h);         ///< Resize the window.
#endif
  };

  /// Re-draw the vb::AutoWindow passed as parameter.
  void AutoWindow_update (void * AW);
}

#endif
