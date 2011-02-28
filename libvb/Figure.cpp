/// @file
/// Implementation of the vb::Figure and related classes.

#include <vb/Figure.h>

namespace vb {
  bool Segment::operator== (const Shape &other) const {
    if (typeid(*this) != typeid(other)) return false;

    Segment *o = (Segment*)(&other);
    if (z1 != o->z1) return false;
    if (z2 != o->z2) return false;

    return true;
  }

  void Segment::draw (Cairo::RefPtr<Cairo::Context> cr){
      cr->move_to (z1.real(), z1.imag());
      cr->line_to (z2.real(), z2.imag());
  }

  /*********************************************************/

  bool Dot::operator== (const Shape &other) const {
    if (typeid(*this) != typeid(other)) return false;

    Dot *o = (Dot*)(&other);
    if (z != o->z) return false;

    return true;
  }

  void Dot::draw (Cairo::RefPtr<Cairo::Context> cr) { }

  /*********************************************************/

  bool Circle::operator== (const Shape &other) const {
    if (typeid(*this) != typeid(other)) return false;

    Circle *o = (Circle*)(&other);
    if (z != o->z) return false;
    if (r != o->r) return false;

    return true;
  }

  void Circle::draw (Cairo::RefPtr<Cairo::Context> cr) {
    cr->begin_new_sub_path ();
    cr->arc (z.real(), z.imag(), r, 0, 2*M_PI);
  }

  /*********************************************************/

  Figure::Figure (bool _o) : AutoWindow (600,600,"Figure"), ortho(_o) { }

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

  Figure & Figure::segment (cpx z1, cpx z2) {
    return add (new Segment (z1,z2));
  }

  Figure & Figure::dot (cpx z, std::string l) {
    return add (new Dot (z,l));
  }

  Figure & Figure::circle (cpx z, double r) {
    return add (new Circle (z,r));
  }

  void Figure::paint (Cairo::RefPtr<Cairo::Context> cr) {
    double w = right()-left(), mid_x = (right()+left())/2;
    double h = top()-bottom(), mid_y = (top()+bottom())/2;

    double scale_x = width/w, scale_y = height/h;
    double scale = min(scale_x,scale_y);
    if (ortho) scale_x = scale_y = scale;

    cr->save();
    cr->set_source_rgb (1,1,1);
    cr->paint();
    
    cr->translate      (width/2,height/2);
    cr->scale          (scale_x,-scale_y);
    cr->translate      (-mid_x,-mid_y);
    cr->set_source_rgb (0,0,0);
    cr->set_line_width (1.0/scale);
    
    foreach (Shape *i, contents) i->draw(cr);
    
    cr->stroke();
    cr->restore();
  }

  void Figure::paint () {
    paint (cr);
  }

  void Figure::output_pdf (const std::string &s) {
    std::ostringstream os;
    if (s == "") os << "output/" << title; else os << s;
    os << ".pdf";

    Cairo::RefPtr<Cairo::PdfSurface> pdf = Cairo::PdfSurface::create (os.str(), width, height);
    Cairo::RefPtr<Cairo::Context>    pcr = Cairo::Context::create (pdf);
    paint (pcr);
    pcr->show_page();
  }

  void Figure::output (const std::string &s) { output_pdf (s); }

  void Figure::unique() {
    std::list<Shape*>::iterator i,j;

    for (i = contents.begin(); i != contents.end(); ++i)
      for (j = i, ++j; j != contents.end(); ++j)
        while ((j != contents.end()) && ((**i) == (**j)))
          j = contents.erase(j);
  }
}
