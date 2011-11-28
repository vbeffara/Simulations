/// @file
/// Definitions for the vb::Figure and related classes.
/// @example test_figure.cpp

#ifndef __VB_FIGURE_H
#define __VB_FIGURE_H

#include <vb/Picture.h>

namespace vb {

  /// Class to store the features of a picture element.
  class Pen {
  public:
    /// The constructor:
    Pen (Color c_=0, double w_=1.0, Color f_=255, bool ff_=false)
      : c(c_), f(f_), w(w_), ff(ff_) { }

    Color c;  ///< The color of the stroke.
    Color f;  ///< The fill color (when applicable).
    double w; ///< The stroke width.
    bool ff;  ///< Whether the shape is filled.
  };

  /// Base class for the elements of a figure.
  class Shape {
  public:
    /// The constructor.
    Shape (Pen p_) : p(p_) { }

    virtual ~Shape() {} ///< Empty destructor to make the compiler happy.

    virtual double left () =0;    ///< Get the left boundary of the Shape.
    virtual double right () =0;   ///< Get the right boundary of the Shape.
    virtual double top () =0;     ///< Get the top boundary of the Shape.
    virtual double bottom () =0;  ///< Get the bottom boundary of the Shape.

    virtual void draw (Cairo::RefPtr<Cairo::Context> cr) =0;  ///< Draw the shape on a Cairo context.

    Pen p; ///< The vb::Pen to use to draw the current vb::Shape.
  };

  /// Subclass of vb::Shape foe a line segment.
  class Segment : public Shape {
  public:
    /// The constructor.
    Segment (cpx zz1, cpx zz2, Pen p = Color(0)) : Shape(p), z1(zz1), z2(zz2) {}

    double left ()   { return min(z1.real(),z2.real()); } ///< Get the left boundary of the Shape.
    double right ()  { return max(z1.real(),z2.real()); } ///< Get the right boundary of the Shape.
    double top ()    { return max(z1.imag(),z2.imag()); } ///< Get the top boundary of the Shape.
    double bottom () { return min(z1.imag(),z2.imag()); } ///< Get the bottom boundary of the Shape.

    void draw (Cairo::RefPtr<Cairo::Context> cr); ///< Draw the shape on a Cairo context.

  private:
    cpx z1,z2;
  };

  /// Subclass of vb::Shape for a dot.
  class Dot : public Shape {
  public:
    /// The constructor.
    Dot (cpx zz, Pen p = Color(0), std::string ll = "") : Shape(p), z(zz), l(ll) {}

    double left ()   { return z.real(); } ///< Get the left boundary of the Shape.
    double right ()  { return z.real(); } ///< Get the right boundary of the Shape.
    double top ()    { return z.imag(); } ///< Get the top boundary of the Shape.
    double bottom () { return z.imag(); } ///< Get the bottom boundary of the Shape.

    void draw (Cairo::RefPtr<Cairo::Context> cr); ///< Draw the shape on a Cairo context.

  private:
    cpx z;
    std::string l;
  };

  /// Subclass of vb::Shape for a circle.
  class Circle : public Shape {
  public:
    /// The constructor.
    Circle (cpx z_, double r_, Pen p = Color(0)) : Shape(p), z(z_), r(r_) {}

    double left ()   { return z.real() - r; } ///< Get the left boundary of the Shape.
    double right ()  { return z.real() + r; } ///< Get the right boundary of the Shape.
    double top ()    { return z.imag() + r; } ///< Get the top boundary of the Shape.
    double bottom () { return z.imag() - r; } ///< Get the bottom boundary of the Shape.

    void draw (Cairo::RefPtr<Cairo::Context> cr); ///< Draw the shape on a Cairo context.

  public:
    cpx z;    ///< The center.
    double r; ///< The radius.
  };

  /// Subclass of vb::Shape for a path.
  class Path : public Shape {
  public:
    /// The constructor.
    Path (std::vector<cpx> z_, Pen p = Color(0)) : Shape(p), z(z_) {}

    double left ();   ///< Get the left boundary of the Shape.
    double right ();  ///< Get the right boundary of the Shape.
    double top ();    ///< Get the top boundary of the Shape.
    double bottom (); ///< Get the bottom boundary of the Shape.

    void draw (Cairo::RefPtr<Cairo::Context> cr); ///< Draw the shape on a Cairo context.

    std::vector<cpx> z; ///< The location of the vertices of the vb::Path.
  };

  /// Subclass of vb::Path for a closed polygon.
  class Polygon : public Path {
  public:
    /// The constructor.
    Polygon (std::vector<cpx> z, Pen p = Color(0)) : Path(z,p) {}

    void draw (Cairo::RefPtr<Cairo::Context> cr); ///< Draw the shape on a Cairo context.
  };


  /** The main Figure class.
   *
   * Right now it's a list of shapes with an output routine.
   */

  class Figure : public Picture {
  public:
    Figure (bool ortho = true); ///< Constructor, reserves a window for display.
    ~Figure ();                 ///< Destructor, also destroys the contents nodes.
    void clean ();              ///< Destroy all the contents nodes.

    double left ();             ///< Get the left boundary of the Figure.
    double right ();            ///< Get the right boundary of the Figure.
    double top ();              ///< Get the top boundary of the Figure.
    double bottom ();           ///< Get the bottom boundary of the Figure.

    Figure &add (Shape *S);     ///< Add an element to the figure.

    std::list<Shape*> contents; ///< The elements of the figure.

    void draw (Cairo::RefPtr<Cairo::Context> cr); ///< Draw it onto a Cairo context.

    /** Output the image as a PDF file.
     *
     * @param s The base name of the output file, defaults to Image::title.
     */

    virtual void output (const std::string &s = "");

    /** Output the image as a PDF file.
     *
     * @param s The base name of the output file, defaults to Image::title.
     */

    void output_pdf (const std::string &s = "");

  protected:
    /// Update the contents of AutoWindow::stage.
    void paint ();
    /// Draw the contents of the figure to a Cairo context.
    void paint (Cairo::RefPtr<Cairo::Context> cr, bool fill = true);

    bool ortho; ///< Whether to force the unit square to be a square.

    double basewidth; ///< Width in real plane to get 1px.
  };
}

#endif
