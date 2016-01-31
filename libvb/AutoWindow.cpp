/// @file
/// Implementation of the vb::AutoWindow class

#include <vb/AutoWindow.h>
#include <vb/Hub.h>
#include <FL/Fl.H>
#include <future>

namespace vb {
	void close_window (Fl_Widget *) { exit(1); }

	AutoWindow::AutoWindow (int wd, int ht) : Auto(.05), Fl_Gl_Window (wd, ht), paused (false) {
		label (H.title.c_str());
		callback(close_window);
	}

	void AutoWindow::show () {
		Fl_Gl_Window::show();
		update();
	}

	void AutoWindow::run (std::function<void()> f) {
		auto ff = std::async(std::launch::async,f);
		while (ff.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) update();
		ff.get();
	}

	int AutoWindow::handle (int event) {
		if (event == FL_KEYDOWN) switch (Fl::event_key()) {
			case 'q': exit (0);                  	break;
			case 'x': exit (1);                  	break;
			case ' ': paused = !paused; return 1;	break;
		}
		return Fl_Gl_Window::handle(event);
	}

	void AutoWindow::update () {
		if (visible()) {
			redraw();
			Fl::check();
			while (paused) Fl::wait();
		}
	}
}
