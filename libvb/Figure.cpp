/// @file Figure.cpp
/// Implementation of the vb::Figure and related classes.

#include <vb/Figure.h>

namespace vb {

  bool Shape::operator== (const Shape&) const {
    return false;
  }

  std::ostream & Shape::printASY (std::ostream &os) {
    not_impl ("Asymptote"); return os;
  }

  std::ostream & Shape::printEPS (std::ostream &os) {
    not_impl ("EPS"); return os;
  }

  std::ostream & Shape::printMP (std::ostream &os) {
    not_impl ("MetaPost"); return os;
  }

  void Shape::not_impl (std::string s) {
    std::ostringstream os;
    os << s << " output not implemented by class " << typeid(*this).name() << std::flush;
    throw (std::runtime_error(os.str()));
  }

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

  Figure::~Figure () {
    std::list<Shape*>::iterator i;
    for (i=contents.begin(); i!=contents.end(); ++i)
      delete *i;
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
