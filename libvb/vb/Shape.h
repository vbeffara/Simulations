#pragma once /// @file
#include <vb/Color.h>
#include <vb/cpx.h>
#include <cairo.h>
#include <vector>

namespace vb {
	class Pen { public:
		Pen (Color c_=0, double w_=1.0, Color f_=255, bool ff_=false) : c(c_), f(f_), w(w_), ff(ff_) { }
		Color c,f; double w; bool ff;
	};

	class Shape { public:
		Shape (Pen p_) : p(p_) { }
		virtual ~Shape() = default;

		virtual double left () =0;
		virtual double right () =0;
		virtual double top () =0;
		virtual double bottom () =0;
		virtual void draw (cairo_t * cr) =0;

		Pen p;
	};

	class Segment : public Shape { public:
		Segment (cpx zz1, cpx zz2, Pen p = Pen()) : Shape(p), z1(zz1), z2(zz2) {}

		double left ()   override { return std::min(z1.real(),z2.real()); }
		double right ()  override { return std::max(z1.real(),z2.real()); }
		double top ()    override { return std::max(z1.imag(),z2.imag()); }
		double bottom () override { return std::min(z1.imag(),z2.imag()); }
		void draw (cairo_t * cr) override;

		cpx z1,z2;
	};

	class Dot : public Shape { public:
		Dot (cpx zz, Pen p = Pen()) : Shape(p), z(zz) {}

		double left ()   override { return z.real(); }
		double right ()  override { return z.real(); }
		double top ()    override { return z.imag(); }
		double bottom () override { return z.imag(); }
		void draw (cairo_t * cr) override;

		cpx z;
	};

	class Circle : public Shape { public:
		Circle (cpx z_, double r_, Pen p = Pen()) : Shape(p), z(z_), r(r_) {}

		double left ()   override { return z.real() - r; }
		double right ()  override { return z.real() + r; }
		double top ()    override { return z.imag() + r; }
		double bottom () override { return z.imag() - r; }
		void draw (cairo_t * cr) override;

		cpx z; double r;
	};

	class Path : public Shape { public:
		Path (std::vector<cpx> z_, Pen p = Pen()) : Shape(p), z(std::move(z_)) {}

		double left () override;
		double right () override;
		double top () override;
		double bottom () override;
		void draw (cairo_t * cr) override;

		std::vector<cpx> z;
	};

	class Polygon : public Path { public:
		Polygon (std::vector<cpx> z, Pen p = Pen()) : Path(z,p) {}
		void draw (cairo_t * cr) override;
	};
}
