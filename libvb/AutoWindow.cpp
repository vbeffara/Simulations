/// @file
/// Implementation of the vb::AutoWindow class

#include <vb/AutoWindow.h>

namespace vb {
  void AutoWindow_update   (void * AW) { ((AutoWindow*)AW) -> update();   }

#ifdef HAVE_FLTK
  void close_window (Fl_Widget *w) { exit(1); }
#endif

  AutoWindow::AutoWindow (int wd, int ht, const std::string &t) :
#ifdef HAVE_FLTK
    Fl_Double_Window (wd, ht, t.c_str()),
#endif
    title(t), width(wd), height(ht), fps(20),

    snapshot_prefix("snapshot"), snapshot_number(0), snapshot_period(0.0), snapshot_task(-1),
    paused (false) {
#ifdef HAVE_FLTK
    task = global_clock.add (5,AutoWindow_update,this);
    callback(close_window);
#endif
  }

  AutoWindow::~AutoWindow () {
    global_clock.remove(task);
    global_clock.remove(snapshot_task);
  }

  void AutoWindow::resize (int w, int h) {
#ifdef HAVE_FLTK
    Fl_Double_Window::size (w,h);
#endif
    width=w; height=h;
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
            // snapshot();
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
    // fl_draw_image (draw_cb,this,0,0,width,height);
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
}
