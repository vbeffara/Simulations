#pragma once /// \file

#include <vb/Auto.h>
#include <FL/Fl_Double_Window.H>

namespace vb {
	class AutoWindow : public Auto, public Fl_Double_Window {
	public:
		AutoWindow (int wd, int ht);

		void show ();
		void update ();
		void pause() { paused=true; update(); }

		int handle (int event);

	private:
		bool paused;
	};
}
