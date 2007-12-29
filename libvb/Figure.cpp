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

  void Segment::draw () {
    fltk::drawline ((float)z1.real(),(float)z1.imag(),(float)z2.real(),(float)z2.imag());
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

  void Dot::draw () { }

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

  void Circle::draw () {
    float x = z.real();
    float y = z.imag();
    fltk::addarc (x-r,y-r,2*r,2*r,0,360);
    fltk::strokepath();
  }

  Figure::Figure () : fltk::Window (400,400,"Figure") { }

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

  void Figure::draw() {
    fltk::setcolor (fltk::WHITE);
    fltk::fillrect (0,0,w(),h());

    fltk::push_matrix();
    fltk::scale(w()/1.0f, h()/1.0f);
    //fltk::translate(1,1);

    fltk::setcolor (fltk::BLACK);
    for (std::list<Shape*>::iterator i = contents.begin(); i != contents.end(); ++i)
      (*i)->draw();

    fltk::pop_matrix();
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
