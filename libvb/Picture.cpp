/// @file
#include <vb/Picture.h>
#include <FL/fl_draw.H>
#include <FL/gl.h>
#include <iostream>
#include <iomanip>
#include <sstream>

namespace vb {
	Picture::Picture (int wd, int ht) :
		AutoWindow (wd, ht), snapshot_prefix(H.title), snapshot_number(0), snapshot_period(0.0), snapshot_task(-1)
		{}

	Picture::~Picture () {
		if (snapshot_task>=0) remove_task(snapshot_task);
	}

	void Picture::show() {
		AutoWindow::show();
		if (!surface) {
			surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, pixel_w(), pixel_h());
			cr = cairo_create (surface);
			cairo_scale (cr,pixel_w()/w(),pixel_h()/h());
			stride = cairo_image_surface_get_stride (surface) / sizeof(Color);
		}
	}

	void Picture::draw () {
		if (!surface) {
			surface = cairo_image_surface_create (CAIRO_FORMAT_RGB24, pixel_w(), pixel_h());
			cr = cairo_create (surface);
			cairo_scale (cr,pixel_w()/w(),pixel_h()/h());
			stride = cairo_image_surface_get_stride (surface) / sizeof(Color);
		}
		paint();
		glPixelStorei (GL_UNPACK_ROW_LENGTH, stride);
		glDrawPixels (pixel_w(),pixel_h(), GL_BGRA,GL_UNSIGNED_BYTE, (const ulong*) cairo_image_surface_get_data(surface));
	}

	void Picture::output_png (const std::string &s) {
		paint();
		std::string os = H.dir + (s == "" ? H.title : s) + ".png";
		cairo_surface_write_to_png (surface, os.c_str());
	}

	void Picture::output (const std::string &s) { output_png (s); }

	void Picture::snapshot () {
		std::ostringstream fn_s;
		fn_s << "snapshots/" << snapshot_prefix << "_" << std::setw(4) << std::setfill('0') << snapshot_number++;
		std::string fn = fn_s.str();

		std::cerr << "Taking a snapshot as " << fn << ".png" << std::endl;
		output_png (fn);
	}

	void Picture::snapshot_setup (const std::string &prefix, double period) {
		if (snapshot_task>=0) remove_task(snapshot_task);
		snapshot_period = period;
		snapshot_prefix = prefix;
		snapshot();
		if (period>0) snapshot_task = add_task (period, [this]{this->snapshot();});
	}

	int Picture::handle (int event) {
		if ((event == FL_KEYDOWN) && (Fl::event_key() == 's')) snapshot();
		return AutoWindow::handle (event);
	}
}
