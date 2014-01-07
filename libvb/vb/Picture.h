#pragma once /// \file

#include <vb/Color.h>
#include <vb/AutoWindow.h>
#include <cairo.h>

namespace vb {
	/** A nice helper class for simulations.
	 *
	 * Subclass of AutoWindow to manage actual pictures (bitmaps, figures
	 * and so on) as opposed to more FLTK-like windows.
	 */

	class Picture : public AutoWindow {
	public:
		Picture (int wd, int ht, const std::string &t = "Picture");
		~Picture ();

		/// Resize the window.
		void size (int w, int h);

		/// Output the image in the preferred format (PNG by default).
		virtual void output (const std::string &s = "");
		void output_png (const std::string &s = "");

		void snapshot_setup (const std::string &prefix, double period = 0.0);
		void snapshot ();

		virtual int handle (int event);	///< Handle the events, in particular 'q' and 'x'.

	protected:
		cairo_surface_t * surface;	///< Cairo version of the surface.
		cairo_t * cr;             	///< A context to draw onto the surface.
		int stride;               	///< The number of pixels in a line in memory.
		void draw ();             	///< Draw the contents of the window (called by FLTK).

	private:
		std::string snapshot_prefix;   ///< The filename prefix for PNG snapshots.
		unsigned int snapshot_number;  ///< The number of the current snapshot.
		double snapshot_period;        ///< The time interval between automatic snapshots, in seconds.
		int snapshot_task;

		virtual void paint () =0;      /// Update the contents of surface from a derived class data.

		/// Shuffle the bytes for FLTK display of a line.
		friend void draw_cb (void *, int, int, int, unsigned char *);
	};
}
