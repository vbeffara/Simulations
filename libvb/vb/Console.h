#pragma once /// \file
#include <vb/AutoWindow.h>
#include <boost/lexical_cast.hpp>
#include <FL/Fl_Hor_Nice_Slider.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Window.H>

namespace vb {
	class Slot : public Fl_Output {
	public:
		Slot (const char *n, int w, int h) : Fl_Output (0,h,w-100,30,n) { align(FL_ALIGN_RIGHT); }
		template <typename T> void draw_value (T t) {
			value(boost::lexical_cast<std::string> (t).c_str()); Fl_Output::draw();
		}
	};

	template <typename T> class Watcher : public Slot {
	public:
		Watcher (T &tt, const char *nn, int w, int h) : Slot (nn,w,h), t(tt) {}
		void draw () override { draw_value(t); }
		T &t;
	};

	template <typename T> class Tracer : public Slot {
	public:
		Tracer (T ff (void*), void *dd, const char *nn, int w, int h) : Slot (nn,w,h), f(ff), d(dd) {}
		void draw () override { draw_value(f(d)); }
		T (*f) (void*);
		void *d;
	};

	template <typename T> class Lambda : public Slot {
	public:
		Lambda (std::function <T()> ff, const char *nn, int w, int h) : Slot (nn,w,h), f(ff) {}
		void draw () override { draw_value(f()); }
		std::function <T()> f;
	};

	template <typename T> void apply (Fl_Widget *W);

	template <typename T> class Manager : public Fl_Hor_Nice_Slider {
	public:
		Manager (T &tt, T t1, T t2, int w, int h) : Fl_Hor_Nice_Slider (0,h,w,30), t(tt) {
			bounds (t1,t2); value (tt); callback (apply<T>);
		}
		T &t;
	};

	template <typename T> void apply (Fl_Widget *W) { Manager<T> *M = (Manager<T> *) W; M->t = M->value(); }

	class Console : public AutoWindow <Fl_Window> {
	public:
		Console () : AutoWindow (400,0) { label("Console"); }

		void add (Fl_Widget *S) { size (w(),h()+S->h()); AutoWindow::add (S); }

		template <typename T> void watch (T &t,                                	const char *n)	{ add (new Watcher<T> (t,n,w(),h())); }
		template <typename T> void trace (T f (void*), void *d,                	const char *n)	{ add (new Tracer<T> (f,d,n,w(),h())); }
		template <typename T> void lambda (std::function <T()> f,const char *n)	{ add (new Lambda<T> (f,n,w(),h())); }
		template <typename T> void manage (T &t, T t1, T t2)                   		{ add (new Manager<T> (t,t1,t2,w(),h())); }
	};
}
