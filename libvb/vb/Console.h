#pragma once /// \file
#include <vb/AutoWindow.h>
#include <boost/lexical_cast.hpp>
#include <FL/Fl_Hor_Nice_Slider.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Window.H>

namespace vb {
	template <typename T> void runoutput (Fl_Widget *W);
	template <typename T> void runinput (Fl_Widget *W);

	class Console : public AutoWindow <Fl_Window> {
	public:
		template <typename T> struct Input : public Fl_Hor_Nice_Slider {
			Input (T t, T t1, T t2, std::function <void(T)> cb, int w, int h) : Fl_Hor_Nice_Slider (0,h,w,30), t(t), f(move(cb)) {
				bounds (t1,t2); value(t); callback (runinput<T>);
			}
			T t;
			std::function <void(T)> f;
		};

		template <typename T> class Manager : public Fl_Hor_Nice_Slider {
		public:
			Manager (T &tt, T t1, T t2, int w, int h) : Fl_Hor_Nice_Slider (0,h,w,30), t(tt) {
				bounds (t1,t2); value (tt); callback (runoutput<T>);
			}
			T &t;
		};

		template <typename T> struct Output : public Fl_Output {
			Output (std::function <T()> ff, const char *nn, int w, int h) : Fl_Output (0,h,w-100,30,nn), f(std::move(ff)) {
				align(FL_ALIGN_RIGHT);
			}
			void draw_value (T t) {
				value(boost::lexical_cast<std::string> (t).c_str()); Fl_Output::draw();
			}
			void draw () override { draw_value(f()); }
			std::function <T()> f;
		};

		Console () : AutoWindow (400,0) { label("Console"); }

		void add (Fl_Widget *S) { size (w(),h()+S->h()); AutoWindow::add (S); }

		template <typename T> void lambda (std::function <T()> f, const char *n) { add (new Output<T> (f,n,w(),h())); }

		template <typename T> void watch (T &t, const char *n) { lambda<T> ([&t]{ return t; }, n); }

		template <typename T> void manage (T &t, T t1, T t2) { add (new Manager<T> (t,t1,t2,w(),h())); }

		void input (double t, double t1, double t2, std::function<void(double)> f) {
			add (new Input<double> (t,t1,t2,move(f),w(),h()));
		}
	};

	template <typename T> void runinput (Fl_Widget *W) {
		auto M = static_cast <Console::Input<T>*> (W); M->t = M->value(); M->f(M->t);
	}

	template <typename T> void runoutput (Fl_Widget *W) {
		auto M = static_cast <Console::Manager<T>*> (W); M->t = M->value();
	}
}
