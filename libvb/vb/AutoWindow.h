/// @file AutoWindow.h
/// Definitions for the vb::AutoWindow class.

#ifndef AUTOWINDOW_H
#define AUTOWINDOW_H

#include <stdlib.h>
#include <time.h>
#include <string>

#ifdef LIBVB_FLTK
#include <FL/Fl.h>
#include <FL/Fl_Double_Window.h>
#include <FL/fl_draw.h>
#include <FL/Enumerations.h>
#endif

#include <png.h>

namespace vb {

  /** A nice helper class for simulations.
   *
   * The window should take care of auto-updating, catching keypresses, 
   * and changing the display approximately 25 times per second. Just 
   * call step() often enough and provide a draw() method in your 
   * sub-class, that should be enough.
   */

#ifdef LIBVB_FLTK
  class AutoWindow : public Fl_Double_Window {
    public:
      
      /** The standard constructor
       *
       * @param wd The width of the window.
       * @param ht The height of the window.
       * @param t  The title of the window.
       */

      AutoWindow (int wd, int ht, std::string t);

      /// Show the window on the screen.
      void show ();

      /// Increment the clock and call cycle() as needed.
      void step() {
	  ++npts;
	  --timer;
	  if (timer==0) cycle();
      }

      /// Update the screen, handle the events.
      void update ();

      /// The thing to do if the user presses 'q'.
      virtual void on_quit() { };

      /// Output the current image to a PNG file of specified name.
      void output_png (std::string s);

      /// Take a snapshot of the current window as a PNG file.
      void snapshot (bool silent = false);

      /// Initiate automatic snapshots.
      void snapshot_setup (std::string prefix, double period = 0.0);

      int fps; ///< The target FPS rate.

    private:
      unsigned long long npts;       ///< The number of actions done since the beginning of time.
      unsigned long delay;
      unsigned long timer;
      unsigned long saved_clock;
      unsigned long long nb_clock;

      std::string snapshot_prefix;   ///< The filename prefix for PNG snapshots.
      unsigned int snapshot_number;  ///< The number of the current snapshot.
      double snapshot_period;        ///< The time interval between automatic snapshots, in seconds.
      unsigned long snapshot_clock;  ///< The instant of the last snapshot taken.

      bool paused;

      /// Estimate the refresh rate, then call update().
      void cycle();

      /// Handle the events, in particular 'q' and 'x'.
      int handle (int event);

      /// Return a pointer to 8bpp image data (for PNG output)
      virtual unsigned char * image_data () { return (unsigned char*) NULL; };
  };
#else
  class AutoWindow {
    public:
      int _w,_h;
      int fps;
      AutoWindow (int w, int h, std::string) : _w(w), _h(h) { }
      int w() { return _w; }
      int h() { return _h; }
      virtual ~AutoWindow () { }
      bool visible() { return false; }
      void show() { }
      void step() { }
      void update() { }
      virtual void on_quit() { };
      virtual unsigned char * image_data () { return (unsigned char*) NULL; };
      void output_png (std::string s);
      void snapshot (bool silent = false);
      void snapshot_setup (std::string prefix, double period = 0.0);
      std::string snapshot_prefix;
      unsigned int snapshot_number;
      double snapshot_period;
      unsigned long snapshot_clock;
  };
#endif
}

#endif
