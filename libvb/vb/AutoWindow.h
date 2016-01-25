#pragma once /// \file

#include <vb/Auto.h>
#include <FL/Fl_Gl_Window.H>

namespace vb {
	class AutoWindow : public Auto, public Fl_Gl_Window {
	public:
		AutoWindow (int wd, int ht);

		void show ();
		void update ();
		void pause() { paused=true; update(); }

		int handle (int event);

		#ifdef NO_RETINA
		int pixel_w () { return w(); }
		int pixel_h () { return h(); }
		#endif

	private:
		bool paused;
	};
}
