#include <vb/Sphere.h>

namespace vb {
    Sphere::Sphere(const std::string &s, size_t w, std::function<Color(cpx)> ff)
        : Coloring(s, {-1.0, -1.0}, {1.0, 1.0}, w, std::move(ff)) {}

    Sphere::Sphere(const std::string &s, size_t w, const std::function<Color(double, double, double)> &ff)
        : Sphere(s, w, [ff](cpx z) {
              double x1 = real(z), x2 = imag(z), n = x1 * x1 + x2 * x2;
              if (n > 1) return NOCOLOR;
              double const x3 = sqrt(1 - n);
              return ff(x1, x2, x3);
          }) {}

    Sphere::Sphere(const std::string &s, size_t w, const std::function<Color(double, double)> &ff)
        : Sphere(s, w, [ff](cpx z) {
              double x1 = real(z), x2 = imag(z), n = x1 * x1 + x2 * x2;
              if (n > 1) return NOCOLOR;
              double theta = asin(sqrt(n)), phi = atan2(x1, x2);
              return ff(theta, phi);
          }) {}

    void Sphere::show() {
        Coloring::show();
        for (const auto &ij : coo_range(pixel_size())) {
            cpx const z  = c_to_z(ij);
            Color &c  = at(ij);
            double ff = 1, x1 = real(z), x2 = imag(z), norm2 = x1 * x1 + x2 * x2;
            if (norm2 <= 1) {
                double x3 = sqrt(1 - norm2), y1 = .3, y2 = -.2, y3 = sqrt(1 - y1 * y1 - y2 * y2);
                double const xy = x1 * y1 + x2 * y2 + x3 * y3;
                ff        = xy > 0 ? xy : 0;
                ff        = .1 + .9 * ff * ff;
            }
            ff  = ff * c.a / 255;
            c.r = uint8_t(int(ff * c.r) + 255 - c.a);
            c.g = uint8_t(int(ff * c.g) + 255 - c.a);
            c.b = uint8_t(int(ff * c.b) + 255 - c.a);
            c.a = 255;
        }
        update();
    }
} // namespace vb
