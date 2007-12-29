
/// @file AutoWindow.cpp
/// Implementation of the vb::AutoWindow class

#ifdef LIBVB_FLTK
#include <vb/AutoWindow.h>

#include <fltk/events.h>
#include <fltk/run.h>

namespace vb {
  void close_window (fltk::Widget *w) {
    exit(1);
  }

  AutoWindow::AutoWindow (int wd, int ht, std::string t) : fltk::Window (wd, ht, t.c_str()),
  npts(0), delay(1), timer(1), saved_clock(clock()), nb_clock(0), paused(false) {
    callback(close_window);
  }

  void AutoWindow::show () {
    Window::show();
    update();
  }

  void AutoWindow::cycle () {
    long tmp = clock() - saved_clock;
    if (tmp>=0) nb_clock += tmp+1;
    if (nb_clock < CLOCKS_PER_SEC/5)
      delay *= 2;
    else {
      delay = 1 + npts * (CLOCKS_PER_SEC/20) / nb_clock;
      update();
    }
    timer = delay;
    saved_clock = clock();
  }

  void AutoWindow::update () {
    if (visible()) {
      redraw();
      fltk::check();
      while (paused) fltk::wait();
    }
  }  

  int AutoWindow::handle (int event) {
    switch (event) {
      case fltk::KEY:
        switch (fltk::event_key()) {
          case fltk::EscapeKey:
          case 0x61:             // this is a A (AZERTY for Q)
          case 0x71:             // this is a Q
            on_quit();
            exit (0);
            break;
          case 0x78:             // this is an X
            exit (1);
            break;
          case fltk::SpaceKey:
            paused = !paused;
            break;
        }
        break;
    }
    return 1;
  }
}
#endif
