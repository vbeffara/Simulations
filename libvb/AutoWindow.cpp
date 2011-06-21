/// @file
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
    title(t), width(wd), height(ht), fps(20),

    surface (Cairo::ImageSurface::create (Cairo::FORMAT_RGB24, width, height)),
    stride  (surface -> get_stride() / sizeof(Color)),
    cr      (Cairo::Context::create (surface)),

    npts(0), delay(1), timer(1), saved_clock(clock()), nb_clock(0),
    snapshot_prefix("snapshot"), snapshot_number(0), snapshot_period(0.0), snapshot_clock(clock()), paused (false) {
#ifdef HAVE_FLTK
      callback(close_window);
#endif
  }

  void AutoWindow::resize (int w, int h) {
#ifdef HAVE_FLTK
    Fl_Double_Window::resize (0,0,w,h);
#endif
    width=w; height=h;
    surface = Cairo::ImageSurface::create (Cairo::FORMAT_RGB24, width, height);
    stride = surface -> get_stride() / sizeof(Color);
    cr = Cairo::Context::create (surface);
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

  void draw_cb (void * in, int x, int y, int w, unsigned char * out) {
    AutoWindow & img  = * (AutoWindow*) in;
    Color      * data = (Color*) img.surface -> get_data();

    for (int i=0; i<w; ++i) {
      Color &C = data [x+i + img.stride*y];
      out[3*i] = C.r; out[3*i + 1] = C.g; out[3*i + 2] = C.b;
    }
  }

  void AutoWindow::draw () {
    paint ();
    fl_draw_image (draw_cb,this,0,0,width,height);
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
    long tmp = C.clock() - saved_clock;
    if (tmp>=0) nb_clock += tmp+1;
    if (nb_clock < CLOCKS_PER_SEC/5)
      delay *= 2;
    else {
      delay = 1 + npts * (CLOCKS_PER_SEC/fps) / nb_clock;
      update();
    }

    if ((snapshot_period > 0.0) && (C.clock() - snapshot_clock > CLOCKS_PER_SEC * snapshot_period))
      snapshot(true);

    timer = delay;
    saved_clock = C.clock();
  }

  void AutoWindow::output_png (const std::string &s) {
    paint();

    std::ostringstream os;
    if (s == "") os << "output/" << title; else os << s;
    os << ".png";

    surface->write_to_png (os.str());
  }

  void AutoWindow::output (const std::string &s) { output_png (s); }

  void AutoWindow::snapshot (bool silent) {
    std::ostringstream fn_s;
    fn_s << snapshot_prefix << "_" << std::setw(4) << std::setfill('0') << snapshot_number++;
    std::string fn = fn_s.str();

    if (!silent) std::cerr << "Taking a snapshot as " << fn << ".png" << std::endl;
    output_png (fn);

    snapshot_clock = C.clock();
  }

  void AutoWindow::snapshot_setup (const std::string &prefix, double period) {
    snapshot_period = period;
    snapshot_prefix = prefix;
    snapshot_number = 0;
    if (period>0.0) snapshot (true);
  }
}
