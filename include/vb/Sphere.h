#pragma once
#include <vb/Coloring.h>

namespace vb {
    class Sphere : public Coloring {
    public:
        Sphere(const std::string &s, size_t w, const std::function<Color(double, double, double)> &f); // Cartesian coordinates
        Sphere(const std::string &s, size_t w, const std::function<Color(double, double)> &f);         // Polar coordinates

        void show() override;

    private:
        Sphere(const std::string &s, size_t w, std::function<Color(cpx)> f);
    };
} // namespace vb
