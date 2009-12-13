/// @file AutoWindow.h
/// Definitions for the vb::AutoWindow class.

#ifndef AUTOWINDOW_H
#define AUTOWINDOW_H

#include <vb/common.h>

#ifdef HAVE_FLTK
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_draw.H>
#endif

namespace vb {
  /** A rather stupid class holding color info in Cairo format.
   *
   * It is just a 4-char structure with a few convenience methods for 
   * making it out of components, and it adapts to the endianness of the 
   * machine as determined at compilation time.
   */

  class Color {
    public:
#ifdef VB_LITTLE_ENDIAN
      char b; ///< The blue component.
      char g; ///< The green component.
      char r; ///< The red component.
      char a; ///< The alpha channel (for ARGB32).
#else
      char b; ///< The blue component.
      char g; ///< The green component.
      char r; ///< The red component.
      char a; ///< The alpha channel (for ARGB32).
#endif

      /// Constructor from RGBA values.
      Color (char R, char G, char B, char A=255)
#ifdef VB_LITTLE_ENDIAN
      : b(B), g(G), r(R), a(A)
#else
      : a(A), r(R), g(G), b(B)
#endif
      { }

      /// Constructor from a greyscale value.
      Color (char V)
#ifdef VB_LITTLE_ENDIAN
      : b(V), g(V), r(V), a(255)
#else
      : a(V), r(V), g(V), b(255)
#endif
      { }
  };

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
      int width;               ///< The width of the image, in pixels.
      int height;              ///< The height of the image, in pixels.
      int fps;                 ///< The target FPS rate.

      /** The standard constructor
       *
       * @param wd The width of the window.
       * @param ht The height of the window.
       * @param t  The title of the window.
       * @param c  Whether the image has color or not.
       */

      AutoWindow (int wd, int ht, const std::string &t);

      /// Show the window on the screen.
      void show ();

      /// Update the screen, handle the events.
      void update ();

      /// Increment the clock and call cycle() as needed.
      void step() { ++npts; --timer; if (timer==0) cycle(); }

      /// Put the image on pause, i.e. wait for user input.
      void pause() { paused=true; update(); }

      /// Output the current image to a PNG file of specified name.
      void output_png (const std::string &s = "");

      /// Take a snapshot of the current window as a PNG file.
      void snapshot (bool silent = false);

      /// Initiate automatic snapshots.
      void snapshot_setup (const std::string &prefix, double period = 0.0);

      /// If FLTK is present, run Fl::run(); if not, do nothing.
      void run ();

#ifdef HAVE_FLTK
      int handle (int event);  ///< Handle the events, in particular 'q' and 'x'.
      void draw ();            ///< Draw the contents of the window (called by FLTK).
#endif

    protected:
      Cairo::RefPtr <Cairo::ImageSurface> surface; ///< Cairo surface with the same contents.
      Cairo::RefPtr <Cairo::Context>      cr;      ///< A context to draw onto the surface.

      int stride; ///< The size of one line of the image in memory.

      /// A staging area intended to contain 8bpp grayscale data.
      unsigned char * stage;

      /// A staging area, same as stage but presented as an array of vb::Color
      Color * stage_c;

      /// Update the contents of surface from a derived class data.
      virtual void paint () =0;

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

#ifdef VB_LITTLE_ENDIAN
      friend void draw_cb (void *, int, int, int, unsigned char *);
#endif
  };
}

#endif
