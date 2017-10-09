#pragma once /// @file
#include <cairo.h>
#include <vb/Color.h>
#include <vb/cpx.h>
#include <vector>

namespace vb {
	class Pen { public:
		explicit Pen (Color c_, double w_=1.0, Color f_=WHITE, bool ff_=false) : c(c_), f(f_), w(w_), ff(ff_) { }
		Pen () {}

		Color c=BLACK, f=WHITE;
		double w=1;
		bool ff=false;
	};

	class Shape { public:
		explicit Shape (Pen p_) : p(p_) { }

		Shape (const Shape &) = default;
		Shape (Shape &&) = default;
		virtual ~Shape() = default;
		Shape & operator= (const Shape &) = default;
		Shape & operator= (Shape &&) = default;

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
		explicit Dot (cpx zz, Pen p = Pen()) : Shape(p), z(zz) {}

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
		explicit Path (std::vector<cpx> z_, Pen p = Pen()) : Shape(p), z(std::move(z_)) {}

		double left () override;
		double right () override;
		double top () override;
		double bottom () override;
		void draw (cairo_t * cr) override;

		std::vector<cpx> z;
	};

	class Polygon : public Path { public:
		explicit Polygon (std::vector<cpx> z, Pen p = Pen()) : Path(std::move(z),p) {}
		void draw (cairo_t * cr) override;
	};
} // namespace vb
