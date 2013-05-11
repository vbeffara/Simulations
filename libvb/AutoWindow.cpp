/// @file
/// Implementation of the vb::AutoWindow class

#include <vb/AutoWindow.h>

namespace vb {
	void AutoWindow_update (void * AW) { ((AutoWindow*)AW) -> update(); }

	void close_window (Fl_Widget *w) { exit(1); }

	AutoWindow::AutoWindow (int wd, int ht, const std::string &t) :
		Fl_Double_Window (wd, ht, t.c_str()),
		title(t), paused (false) {
			task = global_clock.add (5,AutoWindow_update,this);
			callback(close_window);
	}

	AutoWindow::~AutoWindow () {
		global_clock.remove(task);
	}

	void AutoWindow::show () {
		Fl_Double_Window::show();
		update();
	}

	int AutoWindow::handle (int event) {
		if (event == FL_KEYDOWN) switch (Fl::event_key()) {
			case 'q': exit (0);                  	break;
			case 'x': exit (1);                  	break;
			case ' ': paused = !paused; return 1;	break;
		}
		return Fl_Double_Window::handle(event);
	}

	void AutoWindow::update () {
		if (visible()) {
			redraw();
			Fl::check();
			while (paused) Fl::wait();
		}
	}
}
