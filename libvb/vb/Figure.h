/// @file Figure.h
/// Definitions for the vb::Figure and related classes.
/// @example test_figure.cpp

#ifndef __VB_FIGURE_H
#define __VB_FIGURE_H

#include <vb/AutoWindow.h>

namespace vb {

  /// Base class for the elements of a figure.

  class Shape {
    public:
      virtual ~Shape() {}                                     ///< Empty destructor to make the compiler happy.

      virtual double left () =0;                              ///< Get the left boundary of the Shape.
      virtual double right () =0;                             ///< Get the right boundary of the Shape.
      virtual double top () =0;                               ///< Get the top boundary of the Shape.
      virtual double bottom () =0;                            ///< Get the bottom boundary of the Shape.

      virtual bool operator== (const Shape&) const =0;        ///< Test for equality between two shapes.

      virtual std::ostream &printASY (std::ostream &os) =0;   ///< Write ASY code for the shape.

      virtual void draw (Cairo::RefPtr<Cairo::Context> cr) =0; ///< Draw the shape on a Cairo context.
  };

  /// Subclass of vb::Shape foe a line segment.

  class Segment : public Shape {
    private:
      cpx z1;                                              ///< First endpoint.
      cpx z2;                                              ///< Second endpoint.

    public:
      Segment (cpx zz1, cpx zz2) : z1(zz1), z2(zz2) {}     ///< Constructor from two complex numbers.

      /// Get the left boundary of the Shape.
      virtual double left () { return min(z1.real(),z2.real()); }

      /// Get the right boundary of the Shape.
      virtual double right () { return max(z1.real(),z2.real()); }

      /// Get the top boundary of the Shape.
      virtual double top () { return max(z1.imag(),z2.imag()); }

      /// Get the bottom boundary of the Shape.
      virtual double bottom () { return min(z1.imag(),z2.imag()); }

      virtual bool operator== (const Shape&) const;        ///< Test for equality between two shapes.

      virtual std::ostream &printASY (std::ostream &os);   ///< Write ASY code for the shape.

      virtual void draw (Cairo::RefPtr<Cairo::Context> cr); ///< Draw the shape on a Cairo context.
  };

  /// Subclass of vb::Shape for a dot.

  class Dot : public Shape {
    private:
      cpx z;                                               ///< The location.
      std::string l;                                       ///< The label of the dot, if any.

    public:
      Dot (cpx zz, std::string ll = "") : z(zz), l(ll) {}  ///< Constructor from a position and a label.

      /// Get the left boundary of the Shape.
      virtual double left () { return z.real(); };

      /// Get the right boundary of the Shape.
      virtual double right () { return z.real(); };

      /// Get the top boundary of the Shape.
      virtual double top () { return z.imag(); };

      /// Get the bottom boundary of the Shape.
      virtual double bottom () { return z.imag(); };

      virtual bool operator== (const Shape&) const;        ///< Test for equality between two shapes.

      virtual std::ostream &printASY (std::ostream &os);   ///< Write ASY code for the shape.

      virtual void draw (Cairo::RefPtr<Cairo::Context> cr); ///< Draw the shape on a Cairo context.
  };

  /// Subclass of vb::Shape for a circle.

  class Circle : public Shape {
    public:
      cpx z;                                               ///< The center.
      double r;                                              ///< The radius.

      Circle (cpx zz, double rr) : z(zz), r(rr) {}           ///< Constructor from a center and a radius.

      /// Get the left boundary of the Shape.
      virtual double left () { return z.real() - r; };

      /// Get the right boundary of the Shape.
      virtual double right () { return z.real() + r; };

      /// Get the top boundary of the Shape.
      virtual double top () { return z.imag() + r; };

      /// Get the bottom boundary of the Shape.
      virtual double bottom () { return z.imag() - r; };

      virtual bool operator== (const Shape&) const;        ///< Test for equality between two shapes.

      virtual std::ostream &printASY (std::ostream &os);   ///< Write ASY code for the shape.

      virtual void draw (Cairo::RefPtr<Cairo::Context> cr); ///< Draw the shape on a Cairo context.
  };

  /** The main Figure class.
   *
   * Right now it's a list of shapes with an output routine.
   */

  class Figure : public AutoWindow {
    public:
      Figure ();                                           ///< Constructor, reserves a window for display.
      ~Figure ();                                          ///< Destructor, also destroys the contents nodes.
      void clean ();                                       ///< Destroy all the contents nodes.

      double left ();                                        ///< Get the left boundary of the Figure.
      double right ();                                       ///< Get the right boundary of the Figure.
      double top ();                                         ///< Get the top boundary of the Figure.
      double bottom ();                                      ///< Get the bottom boundary of the Figure.

      Figure &add (Shape *S);                              ///< Add an element to the figure.

      Figure &segment (cpx z1, cpx z2);                    ///< Add a segment to the figure.
      Figure &dot (cpx z, std::string l = "");             ///< Add a dot to the figure.
      Figure &circle (cpx z, double r);                      ///< Add a circle to the figure.

      std::ostream &printASY (std::ostream &os);           ///< Output as ASY to a stream.
      void printASY (const char *s);                       ///< Output as ASY to a file.

      /** Remove duplicate entries.
       *
       * That's using an ugly n^2 algorithm for now.
       */

      void unique();

      std::list<Shape*> contents;  ///< The elements of the figure.

      virtual void show ();        ///< Show the window on the screen.
#ifdef HAVE_FLTK
      void draw (); ///< Draw it onto the screen.
#endif
      void draw (Cairo::RefPtr<Cairo::Context> cr); ///< Draw it onto a Cairo context.

    protected:
      virtual void paint ();
  };
}

#endif
