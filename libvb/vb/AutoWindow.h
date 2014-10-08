#pragma once /// \file

#include <vb/Auto.h>
#include <FL/Fl_Double_Window.H>
#include <string>

namespace vb {
	class AutoWindow : public Auto, public Fl_Double_Window {
	public:
		AutoWindow (int wd, int ht, const std::string &t);

		void show ();
		void update ();
		void pause() { paused=true; update(); }

		int handle (int event);

		std::string title,prog;

	private:
		bool paused;
	};
}
