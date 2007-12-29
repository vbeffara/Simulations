/// @file AutoWindow.h
/// Definitions for the vb::AutoWindow class.

#ifndef AUTOWINDOW_H
#define AUTOWINDOW_H

#include <stdlib.h>
#include <time.h>
#include <string>

#ifdef LIBVB_FLTK
#include <fltk/Window.h>
#endif

namespace vb {

  /** A nice helper class for simulations.
   *
   * The window should take care of auto-updating, catching keypresses, 
   * and changing the display approximately 25 times per second. Just 
   * call step() often enough and provide a draw() method in your 
   * sub-class, that should be enough.
   */

#ifdef LIBVB_FLTK
  class AutoWindow : public fltk::Window {
    public:
      unsigned long long npts;
      unsigned long delay;
      unsigned long timer;
      unsigned long saved_clock;
      unsigned long long nb_clock;

      bool paused;

      AutoWindow (int wd, int ht, std::string t);
      void show ();
      void update ();
      void step() {
	  ++npts;
	  --timer;
	  if (timer==0) cycle();
      }
      void cycle();
      int handle (int event);
      virtual void on_quit() { };
  };
#else
  class AutoWindow {
    public:
      AutoWindow (int,int,std::string) { }
      bool visible() { return false; }
      void show() { }
      void step() { }
      void update() { }
      virtual void on_quit() { };
  };
#endif
}

#endif
