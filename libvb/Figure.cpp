#include <cairo-pdf.h>
#include <vb/Figure.h>

namespace vb {
	Figure::Figure () : Picture (600,600) {}

	double Figure::left () {
		double l = 0.0;
		for (const auto & i : contents) l = std::min (l,i->left());
		return l-margin;
	}

	double Figure::right () {
		double l = 0.0;
		for (const auto & i : contents) l = std::max (l,i->right());
		return l+margin;
	}

	double Figure::top () {
		double l = 0.0;
		for (const auto & i : contents) l = std::max (l,i->top());
		return l+margin;
	}

	double Figure::bottom () {
		double l = 0.0;
		for (const auto & i : contents) l = std::min (l,i->bottom());
		return l-margin;
	}

	Figure & Figure::add (std::unique_ptr <Shape> && S) {
		contents.push_back(std::move(S));
		return (*this);
	}

	void Figure::paint (cairo_t * cr, bool fill, bool crop) {
		double real_h = w() * (top()-bottom()) / (right()-left());
		double wd = right()-left(), mid_x = (right()+left())/2;
		double ht = top()-bottom(), mid_y = (top()+bottom())/2;

		double scale_x = w()/wd, scale_y = (crop ? real_h : h())/ht;
		double scale = std::min(scale_x,scale_y);
		if (ortho) scale_x = scale_y = scale;
		basewidth = 1.0/scale;

		if (fill) {
			cairo_save(cr);
			cairo_set_source_rgb (cr, 1,1,1);
			cairo_paint(cr);
			cairo_restore(cr);
		}

		cairo_save(cr);
		cairo_translate     	(cr, w()/2, (crop ? real_h : h())/2);
		cairo_scale         	(cr, scale_x*.98, -scale_y*.98); // A tiny margin for stoke width.
		cairo_translate     	(cr, -mid_x,-mid_y);
		cairo_set_line_width	(cr, basewidth);
		cairo_set_line_join 	(cr, CAIRO_LINE_JOIN_ROUND);
		cairo_set_line_cap  	(cr, CAIRO_LINE_CAP_ROUND);

		for (const auto & i : contents) {
			cairo_save(cr);
			cairo_set_source_rgb (cr, i->p.c.r/255.0, i->p.c.g/255.0, i->p.c.b/255.0);
			cairo_set_line_width (cr, basewidth * i->p.w);
			i->draw(cr);
			if (i->p.ff) {
				cairo_stroke_preserve(cr);
				cairo_set_source_rgba (cr, i->p.f.r/255.0, i->p.f.g/255.0, i->p.f.b/255.0, i->p.f.a/255.0);
				cairo_fill(cr);
			}
			cairo_stroke(cr);
			cairo_restore(cr);
		}

		cairo_restore(cr);
	}

	void Figure::paint () {
		paint (cr);
	}

	void Figure::output_pdf (const std::string &s) {
		std::string os = H.dir + (s == "" ? H.title : s) + ".pdf";

		double real_h = w() * (top()-bottom()) / (right()-left());

		cairo_surface_t * pdf = cairo_pdf_surface_create (os.c_str(), w(), real_h);
		cairo_t * pcr = cairo_create (pdf);
		paint (pcr, false, true);
		cairo_show_page (pcr);
		cairo_destroy (pcr);
		cairo_surface_destroy (pdf);
	}

	void Figure::output (const std::string &s) { output_pdf (s); }
} // namespace vb
