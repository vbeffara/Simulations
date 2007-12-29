/// @file Figure.h
/// Definitions for the vb::Figure and related classes.

#ifndef __VB_FIGURE_H
#define __VB_FIGURE_H

#include <vb/types.h>

#include <iostream>
#include <sstream>
#include <fstream>
#include <list>
#include <stdexcept>
#include <string>

#ifdef LIBVB_FLTK
#include <fltk/Window.h>
#include <fltk/draw.h>
#endif

namespace vb {

  /// Base class for the elements of a figure.

  class Shape {
    public:
      virtual ~Shape() {}                                   ///< Empty destructor to make the compiler happy.

      virtual real left () =0;
      virtual real right () =0;
      virtual real top () =0;
      virtual real bottom () =0;

      virtual bool operator== (const Shape&) const =0;      ///< Test for equality between two shapes.

      virtual std::ostream &printASY (std::ostream &os) =0; ///< Write ASY code for the shape.

#ifdef LIBVB_FLTK
      virtual void draw () =0;                              ///< Draw the shape in a window (FLTK).
#endif
  };

  /// Subclass of vb::Shape foe a line segment.

  class Segment : public Shape {
    private:
      cpx z1;                                              ///< First endpoint.
      cpx z2;                                              ///< Second endpoint.
    public:
      Segment (cpx zz1, cpx zz2) : z1(zz1), z2(zz2) {}     ///< Constructor from two complex numbers.

      virtual real left () { return min(z1.real(),z2.real()); }
      virtual real right () { return max(z1.real(),z2.real()); }
      virtual real top () { return max(z1.imag(),z2.imag()); }
      virtual real bottom () { return min(z1.imag(),z2.imag()); }

      virtual bool operator== (const Shape&) const;        ///< Test for equality between two shapes.

      virtual std::ostream &printASY (std::ostream &os);   ///< Write ASY code for the shape.

#ifdef LIBVB_FLTK
      virtual void draw ();                                ///< Draw the shape in a window (FLTK).
#endif
  };

  /// Subclass of vb::Shape for a dot.

  class Dot : public Shape {
    private:
      cpx z;                                               ///< The location.
      std::string l;                                       ///< The label of the dot, if any.
    public:
      Dot (cpx zz, std::string ll = "") : z(zz), l(ll) {}  ///< Constructor from a position and a label.

      virtual real left () { return z.real(); };
      virtual real right () { return z.real(); };
      virtual real top () { return z.imag(); };
      virtual real bottom () { return z.imag(); };

      virtual bool operator== (const Shape&) const;        ///< Test for equality between two shapes.

      virtual std::ostream &printASY (std::ostream &os);   ///< Write ASY code for the shape.

#ifdef LIBVB_FLTK
      virtual void draw ();                                ///< Draw the shape in a window (FLTK).
#endif
  };

  /// Subclass of vb::Shape for a circle.

  class Circle : public Shape {
    private:
      cpx z;                                               ///< The center.
      real r;                                              ///< The radius.
    public:
      Circle (cpx zz, real rr) : z(zz), r(rr) {}           ///< Constructor from a center and a radius.

      virtual real left () { return z.real() - r; };
      virtual real right () { return z.real() + r; };
      virtual real top () { return z.imag() + r; };
      virtual real bottom () { return z.imag() - r; };

      virtual bool operator== (const Shape&) const;        ///< Test for equality between two shapes.

      virtual std::ostream &printASY (std::ostream &os);   ///< Write ASY code for the shape.

#ifdef LIBVB_FLTK
      virtual void draw ();                                ///< Draw the shape in a window (FLTK).
#endif
  };

  /** The main Figure class.
   *
   * Right now it's a list of shapes with an output routine. The only 
   * output format implemented is .asy (Asymptote), but eventually there 
   * will be .eps (postscript) and .mp (metapost).
   */

#ifdef LIBVB_FLTK
  class Figure : public fltk::Window {
    public:
      void draw();             ///< Fill the fltk::Rectangle R with the image contents.
      //int handle (int event);  ///< Handle keyboard events such as 'q', 'x' etc.
#else
  class Figure {
#endif
    public:
      Figure ();                                           ///< Constructor, reserves a window for display.
      ~Figure ();                                          ///< Destructor, also destroys the contents nodes.

      real left ();
      real right ();
      real top ();
      real bottom ();

      Figure &add (Shape *S);                              ///< Add an element to the figure.

      Figure &segment (cpx z1, cpx z2);                    ///< Add a segment to the figure.
      Figure &dot (cpx z, std::string l = "");             ///< Add a dot to the figure.
      Figure &circle (cpx z, real r);                      ///< Add a circle to the figure.

      std::ostream &printASY (std::ostream &os);           ///< Output as ASY to a stream.
      void printASY (const char *s);                       ///< Output as ASY to a file.

      /** Remove duplicate entries.
       *
       * That's using an ugly n^2 algorithm for now.
       */

      void unique();

      std::list<Shape*> contents;  ///< The elements of the figure.
  };
}

/// @example figtest.cpp

#endif
