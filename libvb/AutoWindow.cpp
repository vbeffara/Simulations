
/// @file AutoWindow.cpp
/// Implementation of the vb::AutoWindow class

#include <vb/AutoWindow.h>

namespace vb {
#ifdef HAVE_FLTK
  void close_window (Fl_Widget *w) { exit(1); }
#endif

  AutoWindow::AutoWindow (int wd, int ht, const std::string &t) :
#ifdef HAVE_FLTK
    Fl_Double_Window (wd, ht, t.c_str()),
#endif
    title(t), width(wd), height(ht), fps(20), npts(0), delay(1), timer(1),
    saved_clock(clock()), nb_clock(0), snapshot_prefix("snapshot"),
    snapshot_number(0), snapshot_period(0.0), snapshot_clock(clock()),
    paused(false) {
#ifdef HAVE_FLTK
    callback(close_window);
#endif
    surface = Cairo::ImageSurface::create (Cairo::FORMAT_RGB24, wd, ht);
    cr      = Cairo::Context::create (surface);
    stride  = surface->get_stride();
    stage   = surface->get_data();
  }

  void AutoWindow::show () {
#ifdef HAVE_FLTK
    Fl_Double_Window::show();
    update();
#else
    std::cerr << "libvb: without FLTK, I can't show you this !" << std::endl;
#endif
  }

  void AutoWindow::run () {
#ifdef HAVE_FLTK
    Fl::run ();
#endif
  }

#ifdef HAVE_FLTK
  int AutoWindow::handle (int event) {
    switch (event) {
      case FL_KEYDOWN:
        switch (Fl::event_key()) {
          case FL_Escape:
          case 0x61:             // this is a A (AZERTY for Q)
          case 0x71:             // this is a Q
            exit (0);
            break;
          case 0x78:             // this is an X
            exit (1);
            break;
          case 0x73:             // this is an S
            snapshot();
            break;
          case 0x20:             // space bar
            paused = !paused;
            break;
        }
        break;
    }
    return 1;
  }

  void AutoWindow::draw () {
    paint ();
    fl_draw_image_mono (stage,0,0,width,height,4,stride);
  }
#endif

  void AutoWindow::update () {
#ifdef HAVE_FLTK
    if (visible()) {
      redraw();
      Fl::check();
      while (paused) Fl::wait();
    }
#endif
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

    if ((snapshot_period > 0.0) && (clock() - snapshot_clock > CLOCKS_PER_SEC * snapshot_period))
      snapshot(true);

    timer = delay;
    saved_clock = clock();
  }

  void AutoWindow::output_png (const std::string &s) {
    paint();
    surface->write_to_png (s);
  }

  void AutoWindow::snapshot (bool silent) {
    std::ostringstream fn_s;
    fn_s << snapshot_prefix << "_" << std::setw(4) << std::setfill('0') << snapshot_number++ << ".png";
    std::string fn = fn_s.str();

    if (!silent) std::cerr << "Taking a snapshot as " << fn << std::endl;
    output_png (fn);

    snapshot_clock = clock();
  }

  void AutoWindow::snapshot_setup (const std::string &prefix, double period) {
    snapshot_period = period;
    snapshot_prefix = prefix;
    snapshot_number = 0;
    if (period>0.0) snapshot (true);
  }
}
