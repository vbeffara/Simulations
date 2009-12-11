/// @file Figure.cpp
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

  std::ostream & Segment::printASY (std::ostream &os) {
    return os << "draw (" << z1 << "--" << z2 << ");" << std::endl;
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

  std::ostream & Dot::printASY (std::ostream &os) {
    if (l.length()) os << "label(\"" << l << "\"," << z << ",NE);";
    return os << "dot(" << z << ");" << std::endl;
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

  std::ostream & Circle::printASY (std::ostream &os) {
    return os << "draw (circle(" << z << "," << r << "));" << std::endl;
  }

  void Circle::draw (Cairo::RefPtr<Cairo::Context> cr) {
    cr->begin_new_sub_path ();
    cr->arc (z.real(), z.imag(), r, 0, 2*M_PI);
  }

  /*********************************************************/

  Figure::Figure () : AutoWindow (400,400,"Figure") { }

  void Figure::clean() {
    std::list<Shape*>::iterator i;
    for (i=contents.begin(); i!=contents.end(); ++i)
      delete *i;
    contents.clear();
  }

  Figure::~Figure () {
    clean();
  }

  double Figure::left () {
    double l = 0.0;
    std::list<Shape*>::iterator i;
    for (i=contents.begin(); i!=contents.end(); ++i)
      l = min (l,(*i)->left());
    return l;
  }

  double Figure::right () {
    double l = 0.0;
    std::list<Shape*>::iterator i;
    for (i=contents.begin(); i!=contents.end(); ++i)
      l = max (l,(*i)->right());
    return l;
  }

  double Figure::top () {
    double l = 0.0;
    std::list<Shape*>::iterator i;
    for (i=contents.begin(); i!=contents.end(); ++i)
      l = max (l,(*i)->top());
    return l;
  }

  double Figure::bottom () {
    double l = 0.0;
    std::list<Shape*>::iterator i;
    for (i=contents.begin(); i!=contents.end(); ++i)
      l = min (l,(*i)->bottom());
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

  void Figure::paint () {
    double width   = right()-left(), mid_x = (right()+left())/2;
    double height  = top()-bottom(), mid_y = (top()+bottom())/2;

    double scale_x = w()/width, scale_y = h()/height;
    double scale = min(scale_x, scale_y);

    cr->save();
      cr->set_source_rgb (1,1,1); cr->paint();

      cr->translate      (w()/2,h()/2);
      cr->scale          (scale,scale);
      cr->translate      (-mid_x,-mid_y);
      cr->set_source_rgb (0,0,0);
      cr->set_line_width (1.0/scale);

      for (std::list<Shape*>::iterator i = contents.begin(); i != contents.end(); ++i)
        (*i)->draw(cr);

      cr->stroke();
    cr->restore();
  }

  std::ostream & Figure::printASY (std::ostream &os) {
    os << "unitsize(1000);" << std::endl;

    std::list<Shape*>::iterator i;
    for (i = contents.begin(); i != contents.end(); ++i) {
      (*i)->printASY(os);
    }
    return os;
  }

  void Figure::printASY (const char *s) {
    std::ofstream f(s);
    printASY(f);
    f.close();
  }

  void Figure::unique() {
    std::list<Shape*>::iterator i,j;

    for (i = contents.begin(); i != contents.end(); ++i)
      for (j = i, ++j; j != contents.end(); ++j)
        while ((j != contents.end()) && ((**i) == (**j)))
          j = contents.erase(j);
  }
}
