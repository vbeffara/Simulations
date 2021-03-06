#include <vb/Sphere.h>

namespace vb {
    Sphere::Sphere(const std::string &s, size_t w, std::function<Color(cpx)> f) : Coloring(s, {-1.0, -1.0}, {1.0, 1.0}, w, std::move(f)) {}

    Sphere::Sphere(const std::string &s, size_t w, const std::function<Color(double, double, double)> &f)
        : Sphere(s, w, [f](cpx z) {
              double x1 = real(z), x2 = imag(z), n = x1 * x1 + x2 * x2;
              if (n > 1) return NOCOLOR;
              double x3 = sqrt(1 - n);
              return f(x1, x2, x3);
          }) {}

    Sphere::Sphere(const std::string &s, size_t w, const std::function<Color(double, double)> &f)
        : Sphere(s, w, [f](cpx z) {
              double x1 = real(z), x2 = imag(z), n = x1 * x1 + x2 * x2;
              if (n > 1) return NOCOLOR;
              double theta = asin(sqrt(n)), phi = atan2(x1, x2);
              return f(theta, phi);
          }) {}

    void Sphere::show() {
        Coloring::show();
        for (const auto &ij : coo_range(pixel_size())) {
            cpx    z = c_to_z(coo(ij));
            Color &c = at(coo(ij));
            double f = 1, x1 = real(z), x2 = imag(z), z2 = x1 * x1 + x2 * x2;
            if (z2 <= 1) {
                double x3 = sqrt(1 - z2), y1 = .3, y2 = -.2, y3 = sqrt(1 - y1 * y1 - y2 * y2);
                double xy = x1 * y1 + x2 * y2 + x3 * y3;
                f         = xy > 0 ? xy : 0;
                f         = .1 + .9 * f * f;
            }
            f   = f * c.a / 255;
            c.r = uint8_t(int(f * c.r) + 255 - c.a);
            c.g = uint8_t(int(f * c.g) + 255 - c.a);
            c.b = uint8_t(int(f * c.b) + 255 - c.a);
            c.a = 255;
        }
        update();
    }
} // namespace vb
