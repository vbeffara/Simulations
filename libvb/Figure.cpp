/// @file Figure.cpp
/// Implementation of the vb::Figure and related classes.

#include <vb/Figure.h>

#include <fstream>

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

#ifdef HAVE_FLTK
  void Segment::draw () {
    fl_begin_line();
    fl_vertex (get_d(z1.real()), get_d(z1.imag()));
    fl_vertex (get_d(z2.real()), get_d(z2.imag()));
    fl_end_line();
  }
#endif

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

#ifdef HAVE_FLTK
  void Dot::draw () { }
#endif

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

#ifdef HAVE_FLTK
  void Circle::draw () {
    fl_circle (get_d(z.real()), get_d(z.imag()), get_d(r));
  }
#endif

  Figure::Figure () : AutoWindow (400,400,"Figure") {
#ifdef HAVE_FLTK
    //resizable (*this);
#endif
  }

  void Figure::clean() {
    std::list<Shape*>::iterator i;
    for (i=contents.begin(); i!=contents.end(); ++i)
      delete *i;
    contents.clear();
  }

  Figure::~Figure () {
    clean();
  }

  Real Figure::left () {
    Real l = 0.0;
    std::list<Shape*>::iterator i;
    for (i=contents.begin(); i!=contents.end(); ++i)
      l = min (l,(*i)->left());
    return l;
  }

  Real Figure::right () {
    Real l = 0.0;
    std::list<Shape*>::iterator i;
    for (i=contents.begin(); i!=contents.end(); ++i)
      l = max (l,(*i)->right());
    return l;
  }

  Real Figure::top () {
    Real l = 0.0;
    std::list<Shape*>::iterator i;
    for (i=contents.begin(); i!=contents.end(); ++i)
      l = max (l,(*i)->top());
    return l;
  }

  Real Figure::bottom () {
    Real l = 0.0;
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

  Figure & Figure::circle (cpx z, Real r) {
    return add (new Circle (z,r));
  }

#ifdef HAVE_FLTK
  void Figure::draw() {
    fl_color (FL_WHITE);
    fl_rectf (0,0,w(),h());

    fl_push_matrix();
    fl_scale (get_d(w()/(right()-left())), get_d(h()/(top()-bottom())));
    fl_translate(get_d(-left()),get_d(-bottom()));

    fl_color (FL_BLACK);
    for (std::list<Shape*>::iterator i = contents.begin(); i != contents.end(); ++i)
      (*i)->draw();

    fl_pop_matrix();
  }
#endif

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
