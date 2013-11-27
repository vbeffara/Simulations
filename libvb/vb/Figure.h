#pragma once /// @file
#include <vb/Picture.h>
#include <vb/common.h>
#include <cairo-pdf.h>
#include <list>
#include <memory>

namespace vb {
	class Pen {
	public:
		Pen (Color c_, double w_=1.0, Color f_=255, bool ff_=false)	: set(true), c(c_), f(f_), w(w_), ff(ff_)  	{ }
		Pen ()                                                     	: set(false), c(0), f(255), w(1), ff(false)	{ };

		bool set; ///< Whether the pen color is set.
		Color c;  ///< The color of the stroke.
		Color f;  ///< The fill color (when applicable).
		double w; ///< The stroke width.
		bool ff;  ///< Whether the shape is filled.
	};

	class Shape {
	public:
		Shape (Pen p_) : p(p_) { }
		virtual ~Shape() {}

		virtual double left () =0; virtual double right () =0; virtual double top () =0; virtual double bottom () =0;
		virtual void draw (cairo_t * cr) =0;

		Pen p;
	};

	class Segment : public Shape {
	public:
		Segment (cpx zz1, cpx zz2, Pen p = Pen()) : Shape(p), z1(zz1), z2(zz2) {}

		double left ()   { return std::min(z1.real(),z2.real()); } ///< Get the left boundary of the Shape.
		double right ()  { return std::max(z1.real(),z2.real()); } ///< Get the right boundary of the Shape.
		double top ()    { return std::max(z1.imag(),z2.imag()); } ///< Get the top boundary of the Shape.
		double bottom () { return std::min(z1.imag(),z2.imag()); } ///< Get the bottom boundary of the Shape.

		void draw (cairo_t * cr); ///< Draw the shape on a Cairo context.

		cpx z1,z2;
	};

	class Dot : public Shape {
	public:
		/// The constructor.
		Dot (cpx zz, Pen p = Pen(), std::string ll = "") : Shape(p), z(zz), l(ll) {}

		double left ()   { return z.real(); } ///< Get the left boundary of the Shape.
		double right ()  { return z.real(); } ///< Get the right boundary of the Shape.
		double top ()    { return z.imag(); } ///< Get the top boundary of the Shape.
		double bottom () { return z.imag(); } ///< Get the bottom boundary of the Shape.

		void draw (cairo_t * cr); ///< Draw the shape on a Cairo context.

	private:
		cpx z;
		std::string l;
	};

	/// Subclass of vb::Shape for a circle.
	class Circle : public Shape {
	public:
		/// The constructor.
		Circle (cpx z_, double r_, Pen p = Pen()) : Shape(p), z(z_), r(r_) {}

		double left ()   { return z.real() - r; } ///< Get the left boundary of the Shape.
		double right ()  { return z.real() + r; } ///< Get the right boundary of the Shape.
		double top ()    { return z.imag() + r; } ///< Get the top boundary of the Shape.
		double bottom () { return z.imag() - r; } ///< Get the bottom boundary of the Shape.

		void draw (cairo_t * cr); ///< Draw the shape on a Cairo context.

	public:
		cpx z;    ///< The center.
		double r; ///< The radius.
	};

	/// Subclass of vb::Shape for a path.
	class Path : public Shape {
	public:
		/// The constructor.
		Path (std::vector<cpx> z_, Pen p = Pen()) : Shape(p), z(z_) {}

		double left ();   ///< Get the left boundary of the Shape.
		double right ();  ///< Get the right boundary of the Shape.
		double top ();    ///< Get the top boundary of the Shape.
		double bottom (); ///< Get the bottom boundary of the Shape.

		void draw (cairo_t * cr); ///< Draw the shape on a Cairo context.

		std::vector<cpx> z; ///< The location of the vertices of the vb::Path.
	};

	/// Subclass of vb::Path for a closed polygon.
	class Polygon : public Path {
	public:
		/// The constructor.
		Polygon (std::vector<cpx> z, Pen p = Pen()) : Path(z,p) {}

		void draw (cairo_t * cr); ///< Draw the shape on a Cairo context.
	};


	/** The main Figure class.
	 *
	 * Right now it's a list of shapes with an output routine.
	 */

	class Figure : public Picture {
	public:
		Figure (bool ortho = true); ///< Constructor, reserves a window for display.

		double left ();             ///< Get the left boundary of the Figure.
		double right ();            ///< Get the right boundary of the Figure.
		double top ();              ///< Get the top boundary of the Figure.
		double bottom ();           ///< Get the bottom boundary of the Figure.

		Figure &add (Shape *S);     ///< Add an element to the figure.

		std::list < std::shared_ptr<Shape> > contents; ///< The elements of the figure.

		void draw (cairo_t * cr); ///< Draw it onto a Cairo context.

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
		void paint ();
		void paint (cairo_t * cr, bool fill = true);

		bool ortho; ///< Whether to force the unit square to be a square.

		double basewidth; ///< Width in real plane to get 1px.
	};
}
