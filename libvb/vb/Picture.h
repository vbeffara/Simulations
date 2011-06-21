/// @file
/// Definitions for the vb::Picture class.

#ifndef VB_PICTURE_H
#define VB_PICTURE_H

#include <vb/Color.h>
#include <vb/AutoWindow.h>

namespace vb {
  /** A nice helper class for simulations.
   *
   * The Picture class takes care of displaying the window on the
   * screen, catching keypresses, and changing the display approximately
   * 25 times per second.  A derived class is expected to do two things:
   *
   * (i) provide a paint() method to fill in Picture::surface
   * (posibly making use of Picture::cr if needed). Updating it
   * elsewhere in the code is safe, because paint() will always be
   * called just before surface() is used;
   *
   * (ii) call Picture::step() often enough, so that the display is
   * updated with the correct frequency. But this is not mandatory.
   */

  class Picture : public AutoWindow {
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
     */

    Picture (int wd, int ht, const std::string &t);
    ~Picture ();

    /// Resize the window.
    void resize (int w, int h);

    /// Show the window on the screen.
    void show ();

    /// Update the screen, handle the events.
    void update ();

    /// Put the image on pause, i.e. wait for user input.
    void pause() { paused=true; update(); }

    /** Output the image in the preferred format (PNG by default).
     *
     * @param s The base name of the output file, defaults to Image::title.
     */

    virtual void output (const std::string &s = "");

    /// Output the current image to a PNG file of specified name.
    void output_png (const std::string &s = "");

    /// Take a snapshot of the current window as a PNG file.
    void snapshot ();

    /// Initiate automatic snapshots.
    void snapshot_setup (const std::string &prefix, double period = 0.0);

    /// If FLTK is present, run Fl::run(); if not, do nothing.
    void run ();

    /// Increment the clock and call cycle() as needed.
    void step() { global_clock.step(); }

  protected:
    Cairo::RefPtr <Cairo::ImageSurface> surface; ///< Cairo surface with the same contents.
    Color * stage;                               ///< The pixel data, presented as a std::vector of vb::Color.
    int stride;                                  ///< The number of pixels in a line in memory.
    Cairo::RefPtr <Cairo::Context>      cr;      ///< A context to draw onto the surface.

  private:
    std::string snapshot_prefix;   ///< The filename prefix for PNG snapshots.
    unsigned int snapshot_number;  ///< The number of the current snapshot.
    double snapshot_period;        ///< The time interval between automatic snapshots, in seconds.
    int snapshot_task;

    bool paused;
    int task;

#ifdef HAVE_FLTK
    int handle (int event);        ///< Handle the events, in particular 'q' and 'x'.
    void draw ();                  ///< Draw the contents of the window (called by FLTK).
#endif

    virtual void paint () =0;      /// Update the contents of surface from a derived class data.

    /// Shuffle the bytes for FLTK display of a line.
    friend void draw_cb (void *, int, int, int, unsigned char *);
  };

  void Picture_update (void * AW);
  void Picture_snapshot (void * AW);
}

#endif
