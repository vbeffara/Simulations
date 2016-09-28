#pragma once /// @file
#include <vb/Picture.h>
#include <vb/Shape.h>

namespace vb {
	class Figure : public Picture { public:
		Figure (bool ortho = true);

		double left (); double right (); double top (); double bottom ();
		Figure &add (Shape *S);

		std::vector <std::unique_ptr<Shape>> contents;

		void output (const std::string &s = "") override;
		void output_pdf (const std::string &s = "");

		double margin = 0;

	protected:
		void paint () override;
		void paint (cairo_t * cr, bool fill = true, bool crop = false);

		bool ortho;
		double basewidth;
	};
}
