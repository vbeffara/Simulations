#pragma once /// \file
#include <vb/Color.h>
#include <vb/AutoWindow.h>
#include <cairo.h>
#include <FL/Fl_Gl_Window.H>

namespace vb {
	/** A nice helper class for simulations.
	 *
	 * Subclass of AutoWindow to manage actual pictures (bitmaps, figures
	 * and so on) as opposed to more FLTK-like windows.
	 */

	class Picture : public AutoWindow <Fl_Gl_Window> {
	public:
		Picture (int wd, int ht);
		~Picture () override;

		Picture (const Picture &) = delete;
		Picture (Picture &&) = delete;
		Picture & operator= (const Picture &) = delete;
		Picture & operator= (Picture &&) = delete;

		/// Output the image in the preferred format (PNG by default).
		virtual void output (const std::string &s = "");
		void output_png (const std::string &s = "");

		void snapshot_setup (const std::string &prefix, double period = 0.0);
		void snapshot ();

		void show() override;
		int handle (int event) override;	///< Handle the events, in particular 'q' and 'x'.

	protected:
		cairo_surface_t * surface = nullptr;	///< Cairo version of the surface.
		cairo_t * cr = nullptr;             	///< A context to draw onto the surface.
		int stride = 0;                     	///< The number of pixels in a line in memory.
		void draw () override;              	///< Draw the contents of the window (called by FLTK).

	private:
		std::string snapshot_prefix;   ///< The filename prefix for PNG snapshots.
		unsigned int snapshot_number;  ///< The number of the current snapshot.
		double snapshot_period;        ///< The time interval between automatic snapshots, in seconds.
		int snapshot_task;

		virtual void paint () {};      /// Update the contents of surface from a derived class data.
	};
} // namespace vb
