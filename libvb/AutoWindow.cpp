
/// @file AutoWindow.cpp
/// Implementation of the vb::AutoWindow class

#include <vb/AutoWindow.h>

namespace vb {
#ifdef LIBVB_FLTK
  /// Forcefully exit the program if the used closes the window.
  void close_window (Fl_Widget *w) {
    exit(1);
  }

  AutoWindow::AutoWindow (int wd, int ht, std::string t) : Fl_Double_Window (wd, ht, t.c_str()),
  fps(20), npts(0), delay(1), timer(1), saved_clock(clock()), nb_clock(0), paused(false) {
    callback(close_window);
  }

  void AutoWindow::show () {
    Fl_Window::show();
    update();
  }

  void AutoWindow::cycle () {
    long tmp = clock() - saved_clock;
    if (tmp>=0) nb_clock += tmp+1;
    if (nb_clock < CLOCKS_PER_SEC/5)
      delay *= 2;
    else {
      delay = 1 + npts * (CLOCKS_PER_SEC/fps) / nb_clock;
      update();
    }
    timer = delay;
    saved_clock = clock();
  }

  void AutoWindow::update () {
    if (visible()) {
      redraw();
      Fl::check();
      while (paused) Fl::wait();
    }
  }  

  int AutoWindow::handle (int event) {
    switch (event) {
      case FL_KEYDOWN:
        switch (Fl::event_key()) {
          case FL_Escape:
          case 0x61:             // this is a A (AZERTY for Q)
          case 0x71:             // this is a Q
            on_quit();
            exit (0);
            break;
          case 0x78:             // this is an X
            exit (1);
            break;
          case 0x20:             // space bar
            paused = !paused;
            break;
        }
        break;
    }
    return 1;
  }
#endif

  void AutoWindow::output_png (std::string s) {
    unsigned char *p = image_data();
    if (!p) return;

    FILE *fp = fopen (s.c_str(),"wb");

    png_structp png_ptr = png_create_write_struct
      (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop info_ptr = png_create_info_struct(png_ptr);

    png_init_io (png_ptr, fp);

    png_set_IHDR (png_ptr, info_ptr, w(), h(), 8, PNG_COLOR_TYPE_GRAY,
        PNG_INTERLACE_NONE,PNG_COMPRESSION_TYPE_DEFAULT,PNG_FILTER_TYPE_DEFAULT);

    png_bytep * row_pointers = new png_bytep [h()];

    for (int i=0; i<h(); ++i)
      row_pointers[i] = p + i*w();
    png_set_rows (png_ptr, info_ptr, row_pointers);

    png_write_info (png_ptr, info_ptr);
    png_write_image (png_ptr, row_pointers);
    png_write_end (png_ptr, NULL);

    fclose (fp);
    delete[] row_pointers;
  }
}
