/// @file
/// Implementation of the vb::Figure and related classes.

#include <vb/Figure.h>

namespace vb {

  void Segment::draw (Cairo::RefPtr<Cairo::Context> cr){
      cr->move_to (z1.real(), z1.imag());
      cr->line_to (z2.real(), z2.imag());
  }

  void Dot::draw (Cairo::RefPtr<Cairo::Context> cr) { }

  void Circle::draw (Cairo::RefPtr<Cairo::Context> cr) {
    cr->begin_new_sub_path ();
    cr->arc (z.real(), z.imag(), r, 0, 2*M_PI);
  }

  double Path::left () {
    double m=z[0].real();
    for (unsigned int i=1; i<z.size(); ++i) m = min (m, z[i].real());
    return m;
  }

  double Path::right () {
    double m=z[0].real();
    for (unsigned int i=1; i<z.size(); ++i) m = max (m, z[i].real());
    return m;
  }

  double Path::top () {
    double m=z[0].imag();
    for (unsigned int i=1; i<z.size(); ++i) m = max (m, z[i].imag());
    return m;
  }

  double Path::bottom () {
    double m=z[0].imag();
    for (unsigned int i=1; i<z.size(); ++i) m = min (m, z[i].imag());
    return m;
  }

  void Path::draw (Cairo::RefPtr<Cairo::Context> cr) {
    cr->move_to (z[0].real(), z[0].imag());
    for (unsigned int i=1; i<z.size(); ++i) cr->line_to (z[i].real(), z[i].imag());
  };

  void Polygon::draw (Cairo::RefPtr<Cairo::Context> cr) {
    cr->move_to (z.back().real(), z.back().imag());
    for (unsigned int i=0; i<z.size(); ++i) cr->line_to (z[i].real(), z[i].imag());
  };

  /*********************************************************/

  Figure::Figure (bool _o) : Picture (600,600,"Figure"), ortho(_o) {}

  void Figure::clean() {
    foreach (Shape *i, contents) delete i;
    contents.clear();
  }

  Figure::~Figure () {
    clean();
  }

  double Figure::left () {
    double l = 0.0;
    foreach (Shape *i, contents) l = min (l,i->left());
    return l;
  }

  double Figure::right () {
    double l = 0.0;
    foreach (Shape *i, contents) l = max (l,i->right());
    return l;
  }

  double Figure::top () {
    double l = 0.0;
    foreach (Shape *i, contents) l = max (l,i->top());
    return l;
  }

  double Figure::bottom () {
    double l = 0.0;
    foreach (Shape *i, contents) l = min (l,i->bottom());
    return l;
  }

  Figure & Figure::add (Shape *S) {
    contents.push_back(S);
    return (*this);
  }

  void Figure::paint (Cairo::RefPtr<Cairo::Context> cr) {
    double wd = right()-left(), mid_x = (right()+left())/2;
    double ht = top()-bottom(), mid_y = (top()+bottom())/2;

    double scale_x = w()/wd, scale_y = h()/ht;
    double scale = min(scale_x,scale_y);
    if (ortho) scale_x = scale_y = scale;
    basewidth = 1.0/scale;

    cr->save();
    cr->set_source_rgb (1,1,1);
    cr->paint();

    cr->translate      (w()/2, h()/2);
    cr->scale          (scale_x*.98, -scale_y*.98); // A tiny margin for stoke width.
    cr->translate      (-mid_x,-mid_y);
    cr->set_line_width (basewidth);
    cr->set_line_join  (Cairo::LINE_JOIN_ROUND);
    cr->set_line_cap   (Cairo::LINE_CAP_ROUND);

    foreach (Shape *i, contents) {
      cr->save();
      cr->set_source_rgb (i->p.c.r/255.0, i->p.c.g/255.0, i->p.c.b/255.0);
      cr->set_line_width (basewidth * i->p.w);
      i->draw(cr);
      if (i->p.ff) {
        cr->stroke_preserve();
        cr->set_source_rgb (i->p.f.r/255.0, i->p.f.g/255.0, i->p.f.b/255.0);
        cr->fill();
      }
      cr->stroke();
      cr->restore();
    }

    cr->restore();
  }

  void Figure::paint () {
    paint (cr);
  }

  void Figure::output_pdf (const std::string &s) {
    std::ostringstream os;
    if (s == "") os << "output/" << title; else os << s;
    os << ".pdf";

    Cairo::RefPtr<Cairo::PdfSurface> pdf = Cairo::PdfSurface::create (os.str(), w(), h());
    Cairo::RefPtr<Cairo::Context>    pcr = Cairo::Context::create (pdf);
    paint (pcr);
    pcr->show_page();
  }

  void Figure::output (const std::string &s) { output_pdf (s); }
}
