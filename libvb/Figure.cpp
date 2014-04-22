/// @file
/// Implementation of the vb::Figure and related classes.

#include <vb/Figure.h>

namespace vb {

  void Segment::draw (cairo_t * cr){
      cairo_move_to (cr, z1.real(), z1.imag());
      cairo_line_to (cr, z2.real(), z2.imag());
  }

  void Dot::draw (cairo_t *) { }

  void Circle::draw (cairo_t * cr) {
    cairo_new_sub_path (cr);
    cairo_arc (cr, z.real(), z.imag(), r, 0, 2*M_PI);
  }

  double Path::left () {
    double m=z[0].real();
    for (unsigned int i=1; i<z.size(); ++i) if (std::isnormal(real(z[i]))) m = std::min (m, z[i].real());
    return m;
  }

  double Path::right () {
    double m=z[0].real();
    for (unsigned int i=1; i<z.size(); ++i) if (std::isnormal(real(z[i]))) m = std::max (m, z[i].real());
    return m;
  }

  double Path::top () {
    double m=z[0].imag();
    for (unsigned int i=1; i<z.size(); ++i) if (std::isnormal(real(z[i]))) m = std::max (m, z[i].imag());
    return m;
  }

  double Path::bottom () {
    double m=z[0].imag();
    for (unsigned int i=1; i<z.size(); ++i) if (std::isnormal(real(z[i]))) m = std::min (m, z[i].imag());
    return m;
  }

  void Path::draw (cairo_t * cr) {
    bool down = false;
    for (auto p : z) {
      if (isnan(real(p))) { down = false; continue; }
      if (!down) { cairo_move_to (cr, p.real(), p.imag()); down = true; }
      else { cairo_line_to (cr, p.real(), p.imag()); }
    }
  };

  void Polygon::draw (cairo_t * cr) {
    cairo_move_to (cr, z.back().real(), z.back().imag());
    for (unsigned int i=0; i<z.size(); ++i) cairo_line_to (cr, z[i].real(), z[i].imag());
  };

  /*********************************************************/

  Figure::Figure (bool _o) : Picture (600,600,"Figure"), ortho(_o) {}

  double Figure::left () {
    double l = 0.0;
    for (std::shared_ptr <Shape> i : contents) l = std::min (l,i->left());
    return l;
  }

  double Figure::right () {
    double l = 0.0;
    for (std::shared_ptr <Shape> i : contents) l = std::max (l,i->right());
    return l;
  }

  double Figure::top () {
    double l = 0.0;
    for (std::shared_ptr <Shape> i : contents) l = std::max (l,i->top());
    return l;
  }

  double Figure::bottom () {
    double l = 0.0;
    for (std::shared_ptr <Shape> i : contents) l = std::min (l,i->bottom());
    return l;
  }

  Figure & Figure::add (Shape *S) {
    contents.push_back(std::shared_ptr<Shape>(S));
    return (*this);
  }

  void Figure::paint (cairo_t * cr, bool fill) {
    double wd = right()-left(), mid_x = (right()+left())/2;
    double ht = top()-bottom(), mid_y = (top()+bottom())/2;

    double scale_x = w()/wd, scale_y = h()/ht;
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
    cairo_translate      (cr, w()/2, h()/2);
    cairo_scale          (cr, scale_x*.98, -scale_y*.98); // A tiny margin for stoke width.
    cairo_translate      (cr, -mid_x,-mid_y);
    cairo_set_line_width (cr, basewidth);
    cairo_set_line_join  (cr, CAIRO_LINE_JOIN_ROUND);
    cairo_set_line_cap   (cr, CAIRO_LINE_CAP_ROUND);

    for (std::shared_ptr <Shape> i : contents) {
      cairo_save(cr);
      cairo_set_source_rgb (cr, i->p.c.r/255.0, i->p.c.g/255.0, i->p.c.b/255.0);
      cairo_set_line_width (cr, basewidth * i->p.w);
      i->draw(cr);
      if (i->p.ff) {
        cairo_stroke_preserve(cr);
        cairo_set_source_rgb (cr, i->p.f.r/255.0, i->p.f.g/255.0, i->p.f.b/255.0);
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
    std::ostringstream os;
    if (s == "") os << "output/" << title; else os << s;
    os << ".pdf";

    cairo_surface_t * pdf = cairo_pdf_surface_create (os.str().c_str(), w(), h());
    cairo_t * pcr = cairo_create (pdf);
    paint (pcr, false);
    cairo_show_page (pcr);
    cairo_destroy (pcr);
    cairo_surface_destroy (pdf);
  }

  void Figure::output (const std::string &s) { output_pdf (s); }
}
