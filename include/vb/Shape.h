#pragma once /// @file
#include <cairo/cairo.h>
#include <vb/Color.h>
#include <vb/util/cpx.h>
#include <vector>

namespace vb {
    class Pen {
    public:
        explicit Pen(Color c_, double w_ = 1.0, Color f_ = WHITE, bool ff_ = false) : c(c_), f(f_), w(w_), ff(ff_) {}
        Pen() = default;

        Color  c = BLACK, f = WHITE;
        double w  = 1;
        bool   ff = false;
    };

    class Shape {
    public:
        explicit Shape(Pen p_) : p(p_) {}

        Shape(const Shape &) = default;
        Shape(Shape &&)      = default;
        virtual ~Shape()     = default;
        auto operator=(const Shape &) -> Shape & = default;
        auto operator=(Shape &&) -> Shape & = default;

        virtual auto left() -> double   = 0;
        virtual auto right() -> double  = 0;
        virtual auto top() -> double    = 0;
        virtual auto bottom() -> double = 0;
        virtual void draw(cairo_t *cr)  = 0;

        Pen p;
    };

    class Segment : public Shape {
    public:
        Segment(cpx zz1, cpx zz2, Pen p_ = Pen()) : Shape(p_), z1(zz1), z2(zz2) {}

        auto left() -> double override { return std::min(z1.real(), z2.real()); }
        auto right() -> double override { return std::max(z1.real(), z2.real()); }
        auto top() -> double override { return std::max(z1.imag(), z2.imag()); }
        auto bottom() -> double override { return std::min(z1.imag(), z2.imag()); }
        void draw(cairo_t *cr) override;

        cpx z1, z2;
    };

    class Dot : public Shape {
    public:
        explicit Dot(cpx zz, Pen p_ = Pen()) : Shape(p_), z(zz) {}

        auto left() -> double override { return z.real(); }
        auto right() -> double override { return z.real(); }
        auto top() -> double override { return z.imag(); }
        auto bottom() -> double override { return z.imag(); }
        void draw(cairo_t *cr) override;

        cpx z;
    };

    class Circle : public Shape {
    public:
        Circle(cpx z_, double r_, Pen p_ = Pen()) : Shape(p_), z(z_), r(r_) {}

        auto left() -> double override { return z.real() - r; }
        auto right() -> double override { return z.real() + r; }
        auto top() -> double override { return z.imag() + r; }
        auto bottom() -> double override { return z.imag() - r; }
        void draw(cairo_t *cr) override;

        cpx    z;
        double r;
    };

    class Path : public Shape {
    public:
        explicit Path(std::vector<cpx> z_, Pen p_ = Pen()) : Shape(p_), z(std::move(z_)) {}

        auto left() -> double override;
        auto right() -> double override;
        auto top() -> double override;
        auto bottom() -> double override;
        void draw(cairo_t *cr) override;

        std::vector<cpx> z;
    };

    class Polygon : public Path {
    public:
        explicit Polygon(std::vector<cpx> z_, Pen p_ = Pen()) : Path(std::move(z_), p_) {}
        void draw(cairo_t *cr) override;
    };
} // namespace vb
