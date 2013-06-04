#pragma once /// \file
#include <vb/Clock.h>
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <string>

namespace vb {
  /** A nice helper class for simulations.
   *
   * The AutoWindow class takes care of displaying the window on the
   * screen, catching keypresses, and changing the display approximately
   * 25 times per second.  A derived class is expected to do two things:
   *
   * (i) provide a paint() method to fill in AutoWindow::surface
   * (posibly making use of AutoWindow::cr if needed).
   *
   * (ii) call AutoWindow::step() often enough, so that the display is
   * updated with the correct frequency. But this is not mandatory.
   */

  class AutoWindow : public Fl_Double_Window {
  public:
    std::string title;       ///< The title of the window.

    AutoWindow (int wd, int ht, const std::string &t);
    ~AutoWindow ();

    void show ();

    virtual void update ();

    void pause() { paused=true; update(); }

    void step() { global_clock.step(); }

    virtual int handle (int event);   ///< Handle the events, in particular 'q' and 'x'.

  private:
    bool paused;
    int task;
  };

  void AutoWindow_update (void * AW);
}
