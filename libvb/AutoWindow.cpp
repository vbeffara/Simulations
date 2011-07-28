/// @file
/// Implementation of the vb::AutoWindow class

#include <vb/AutoWindow.h>

namespace vb {
  void AutoWindow_update   (void * AW) { ((AutoWindow*)AW) -> update(); }

#ifdef HAVE_FLTK
  void close_window (Fl_Widget *w) { exit(1); }
#endif

  AutoWindow::AutoWindow (int wd, int ht, const std::string &t) :
#ifdef HAVE_FLTK
    Fl_Double_Window (wd, ht, t.c_str()),
#endif
    title(t),
#ifndef HAVE_FLTK
    width(wd), height(ht),
#endif
    fps(20),

    paused (false) {
#ifdef HAVE_FLTK
    task = global_clock.add (5,AutoWindow_update,this);
    callback(close_window);
#endif
  }

  AutoWindow::~AutoWindow () {
    global_clock.remove(task);
  }

#ifndef HAVE_FLTK
  void AutoWindow::size (int w, int h) {
    width=w; height=h;
  }
#endif

  void AutoWindow::show () {
#ifdef HAVE_FLTK
    Fl_Double_Window::show();
    update();
#endif
  }

#ifdef HAVE_FLTK
  int AutoWindow::handle (int event) {
    switch (event) {
      case FL_KEYDOWN:
        switch (Fl::event_key()) {
          case FL_Escape:
          case 'q':
            exit (0);
            break;
          case 'x':             // this is an X
            exit (1);
            break;
          case 's':             // this is an S
            // snapshot();
            break;
          case ' ':             // space bar
            paused = !paused;
            break;
        }
        break;
    }
    return 1;
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
