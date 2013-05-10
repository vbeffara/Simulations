/// @file
/// Definitions for the vb::Picture class.

#pragma once
#include <vb/Color.h>
#include <vb/AutoWindow.h>

namespace vb {
	/** A nice helper class for simulations.
	 *
	 * Subclass of AutoWindow to manage actual pictures (bitmaps, figures
	 * and so on) as opposed to more FLTK-like windows.
	 */

	class Picture : public AutoWindow {
	public:
		/** The standard constructor
		 *
		 * @param wd The width of the window.
		 * @param ht The height of the window.
		 * @param t  The title of the window.
		 */

		Picture (int wd, int ht, const std::string &t = "Picture");
		~Picture ();

		/// Resize the window.
		void size (int w, int h);

		/** Output the image in the preferred format (PNG by default).
		 *
		 * @param s The base name of the output file, defaults to Image::title.
		 */

		virtual void output (const std::string &s = "");

		/// Output the current image to a PNG file of specified name.
		void output_png (const std::string &s = "");

		/// Take a snapshot of the current window as a PNG file.
		void snapshot ();

		/// Initiate automatic snapshots.
		void snapshot_setup (const std::string &prefix, double period = 0.0);

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

	/// Take a snapshot of the picture passed as argument.
	void Picture_snapshot (void * AW);
}
