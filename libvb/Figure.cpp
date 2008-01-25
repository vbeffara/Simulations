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

#ifdef LIBVB_FLTK
  void Segment::draw () {
    fl_begin_line();
    fl_vertex ((float) z1.real(), (float) z1.imag());
    fl_vertex ((float) z2.real(), (float) z2.imag());
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

#ifdef LIBVB_FLTK
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

#ifdef LIBVB_FLTK
  void Circle::draw () {
    fl_circle ((float) z.real(), (float) z.imag(), (float) r);
  }
#endif

#ifdef LIBVB_FLTK
  Figure::Figure () : Fl_Window (400,400,"Figure") { }
#else
  Figure::Figure () { }
#endif

  Figure::~Figure () {
    std::list<Shape*>::iterator i;
    for (i=contents.begin(); i!=contents.end(); ++i)
      delete *i;
  }

  real Figure::left () {
    real l = 0.0;
    std::list<Shape*>::iterator i;
    for (i=contents.begin(); i!=contents.end(); ++i)
      l = min (l,(*i)->left());
    return l;
  }

  real Figure::right () {
    real l = 0.0;
    std::list<Shape*>::iterator i;
    for (i=contents.begin(); i!=contents.end(); ++i)
      l = max (l,(*i)->right());
    return l;
  }

  real Figure::top () {
    real l = 0.0;
    std::list<Shape*>::iterator i;
    for (i=contents.begin(); i!=contents.end(); ++i)
      l = max (l,(*i)->top());
    return l;
  }

  real Figure::bottom () {
    real l = 0.0;
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

  Figure & Figure::circle (cpx z, real r) {
    return add (new Circle (z,r));
  }

#ifdef LIBVB_FLTK
  void Figure::draw() {
    fl_color (FL_WHITE);
    fl_rectf (0,0,w(),h());

    fl_push_matrix();
    fl_scale(w()/(float)(right()-left()), h()/(float)(top()-bottom()));
    fl_translate((float)(-left()),(float)(-bottom()));

    fl_color (FL_BLACK);
    for (std::list<Shape*>::iterator i = contents.begin(); i != contents.end(); ++i)
      (*i)->draw();

    fl_pop_matrix();
  }
#endif

  std::ostream & Figure::printASY (std::ostream &os) {
    os << "unitsize(100);" << std::endl;

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
