/// @file
/// Definitions for the vb::Console class

#pragma once
#include <vb/AutoWindow.h>

namespace vb {
	class Slot : public Fl_Output {
	public:
		Slot (const char *n, int w) : Fl_Output (0,0,w-100,30,n) { align(FL_ALIGN_RIGHT); }
		template <typename T> void draw_value (T t) {
			std::ostringstream os; os << t; value(os.str().c_str()); Fl_Output::draw();
		};
	};

	template <typename T> class Watcher : public Slot {
	public:
		Watcher (T &tt, const char *nn, int w) : Slot (nn,w), t(tt) {}
		void draw () { draw_value(t); }
		T &t;
	};

	template <typename T> class Tracer : public Slot {
	public:
		Tracer (T ff (void*), void *dd, const char *nn, int w) : Slot (nn,w), f(ff), d(dd) {}
		void draw () { draw_value(f(d)); }
		T (*f) (void*);
		void *d;
	};

	template <typename T> void apply (Fl_Widget *W);

	template <typename T> class Manager : public Fl_Hor_Nice_Slider {
	public:
		Manager (T &tt, T t1, T t2, int w) : Fl_Hor_Nice_Slider (0,0,w,30), t(tt) {
			bounds (t1,t2); value (tt); callback (apply<T>);
		}
		T &t;
	};

	template <typename T> void apply (Fl_Widget *W) { Manager<T> *M = (Manager<T> *) W; M->t = M->value(); }

	class Console : public AutoWindow {
	public:
		Console () : AutoWindow (400,0,"Console") {}

		void add (Fl_Widget *S) { S->position (0,h()); size (w(),h()+S->h()); AutoWindow::add (S); }

		template <typename T> void watch (T &t,                	const char *n)	{ add (new Watcher<T> (t,n,w())); }
		template <typename T> void trace (T f (void*), void *d,	const char *n)	{ add (new Tracer<T> (f,d,n,w())); }
		template <typename T> void manage (T &t, T t1, T t2)   	              	{ add (new Manager<T> (t,t1,t2,w())); }
	};
}
