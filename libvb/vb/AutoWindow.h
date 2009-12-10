/// @file AutoWindow.h
/// Definitions for the vb::AutoWindow class.

#ifndef AUTOWINDOW_H
#define AUTOWINDOW_H

#include <vb/common.h>

namespace vb {

  /** A nice helper class for simulations.
   *
   * The window should take care of auto-updating, catching keypresses, 
   * and changing the display approximately 25 times per second. Just 
   * call step() often enough and provide a draw() method in your 
   * sub-class, that should be enough.
   */

  class AutoWindow
#ifdef HAVE_FLTK
    : public Fl_Double_Window
#endif
    {
    public:
      virtual void show ();       ///< Show the window on the screen.

#ifdef HAVE_FLTK
      int handle (int event);     ///< Handle the events, in particular 'q' and 'x'.
#else
      int _w,_h;
      int w()                { return _w;    }
      int h()                { return _h;    }
      bool visible()         { return false; }
#endif
      
      /** The standard constructor
       *
       * @param wd The width of the window.
       * @param ht The height of the window.
       * @param t  The title of the window.
       */

      AutoWindow (int wd, int ht, const std::string &t);

      /// Update the screen, handle the events.
      void update ();

      /// Increment the clock and call cycle() as needed.
      void step() { ++npts; --timer; if (timer==0) cycle(); }

      /// Put the image on pause, i.e. wait for user input.
      void pause() { paused=true; update(); }

      /// Output the current image to a PNG file of specified name.
      void output_png (const std::string &s);

      /// Take a snapshot of the current window as a PNG file.
      void snapshot (bool silent = false);

      /// Initiate automatic snapshots.
      void snapshot_setup (const std::string &prefix, double period = 0.0);

      std::string title;             ///< The title of the window.
      int fps;                       ///< The target FPS rate.

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

      std::vector <unsigned char> raw_image_data;

    protected:
      Cairo::RefPtr <Cairo::ImageSurface> surface; ///< Cairo surface with the same contents.

      int stride; ///< The size of one line of the image in memory.

      /// A staging area intended to contain 8bpp grayscale data.
      unsigned char * stage;

      /// Return a pointer to 8bpp image data (for PNG output)
      virtual unsigned char * image_data ();
  };
}

#endif
