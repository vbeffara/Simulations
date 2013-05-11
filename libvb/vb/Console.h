/// @file
/// Definitions for the vb::Console class

#pragma once
#include <vb/AutoWindow.h>

namespace vb {
	template <typename T> class Watcher : public Fl_Group {
	public:
		Watcher (const T &tt, const std::string &nn, int w) : Fl_Group (0,0,w,30), t(tt), n(nn) {
			new Fl_Button (0,0,150,30,n.c_str());
			o = new Fl_Output (150,0,w-150,30);
		}
		void draw () {
			std::ostringstream os; os << t; o->value(os.str().c_str());
			Fl_Group::draw();
		}
		const T &t;
		Fl_Output *o;
		std::string n;
	};

	template <typename T> class Tracer : public Fl_Group {
	public:
		typedef T calc_function (void*);
		Tracer (calc_function ff, void *dd, const std::string &nn, int w) : Fl_Group (0,0,w,30), f(ff), d(dd), n(nn) {
			new Fl_Button (0,0,150,30,n.c_str());
			o = new Fl_Output (150,0,w-150,30);
		}
		void draw () {
			std::ostringstream os; os << f(d); o->value(os.str().c_str());
			Fl_Group::draw();
		}
		calc_function *f;
		void *d;
		Fl_Output *o;
		std::string n;
	};

	class Console : public AutoWindow {
	public:
		Console () : AutoWindow (400,0,"Console") {}

		template <typename T> void watch (const T &t, const std::string &n) {
			Watcher<T> *WW = new Watcher<T> (t,n,w());
			WW->position (0,h()); size (w(), h()+WW->h()); add (WW);
		}

		template <typename T> void trace (T f (void*), void *d, const std::string &n) {
			Tracer<T> *WW = new Tracer<T> (f,d,n,w());
			WW->position (0,h()); size (w(), h()+WW->h()); add (WW);
		};

	private:
		int task;
	};
}
