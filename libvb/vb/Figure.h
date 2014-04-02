#pragma once /// @file
#include <vb/Picture.h>
#include <vb/cpx.h>
#include <cairo-pdf.h>
#include <memory>

namespace vb {
	class Pen {
	public:
		Pen (Color c_=0, double w_=1.0, Color f_=255, bool ff_=false) : c(c_), f(f_), w(w_), ff(ff_) { }
		Color c,f; double w; bool ff;
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

		double left ()   { return std::min(z1.real(),z2.real()); }
		double right ()  { return std::max(z1.real(),z2.real()); }
		double top ()    { return std::max(z1.imag(),z2.imag()); }
		double bottom () { return std::min(z1.imag(),z2.imag()); }
		void draw (cairo_t * cr);

		cpx z1,z2;
	};

	class Dot : public Shape {
	public:
		Dot (cpx zz, Pen p = Pen(), std::string ll = "") : Shape(p), z(zz) {}

		double left ()   { return z.real(); }
		double right ()  { return z.real(); }
		double top ()    { return z.imag(); }
		double bottom () { return z.imag(); }
		void draw (cairo_t * cr);

		cpx z;
	};

	class Circle : public Shape {
	public:
		Circle (cpx z_, double r_, Pen p = Pen()) : Shape(p), z(z_), r(r_) {}

		double left ()   { return z.real() - r; }
		double right ()  { return z.real() + r; }
		double top ()    { return z.imag() + r; }
		double bottom () { return z.imag() - r; }
		void draw (cairo_t * cr);

		cpx z; double r;
	};

	class Path : public Shape {
	public:
		Path (std::vector<cpx> z_, Pen p = Pen()) : Shape(p), z(z_) {}

		double left (); double right (); double top (); double bottom ();
		void draw (cairo_t * cr);

		std::vector<cpx> z;
	};

	class Polygon : public Path {
	public:
		Polygon (std::vector<cpx> z, Pen p = Pen()) : Path(z,p) {}
		void draw (cairo_t * cr);
	};

	class Figure : public Picture {
	public:
		Figure (bool ortho = true);

		double left (); double right (); double top (); double bottom ();
		Figure &add (Shape *S);

		std::vector <std::shared_ptr<Shape>> contents;

		virtual void output (const std::string &s = "");
		void output_pdf (const std::string &s = "");

	protected:
		void paint ();
		void paint (cairo_t * cr, bool fill = true);

		bool ortho;
		double basewidth;
	};
}
