#pragma once /// \file
#include <vb/Auto.h>
#include <vb/Hub.h>
#include <FL/Fl.H>
#include <future>

namespace vb {
	static inline void close_window (Fl_Widget *) { exit(1); }

	template <typename T> class AutoWindow : public Auto, public T { public:
		AutoWindow (int wd, int ht) : Auto(.1), T (wd, ht), paused (false) {
			T::label (H.title.c_str());
			T::callback(close_window);
		}

		void update () { if (T::visible()) { T::redraw(); Fl::check(); while (paused) Fl::wait(); } }

		void show () { T::show(); update(); }

		void pause() { paused=true; update(); }

		void run (std::function<void()> f) {
			auto ff = std::async(std::launch::async,f);
			while (ff.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) update();
			ff.get();
		}

		int handle (int event) {
			if (event == FL_KEYDOWN) switch (Fl::event_key()) {
				case 'q': exit (0);        	         	break;
				case 'x': exit (1);        	         	break;
				case 'h': T::hide();       	return 1;	break;
				case ' ': paused = !paused;	return 1;	break;
			}
			return T::handle(event);
		}

		#ifdef NO_RETINA
		int pixel_w () { return T::w(); }
		int pixel_h () { return T::h(); }
		#endif

	private:
		bool paused;
	};
}
