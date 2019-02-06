#pragma once /// \file
#include <fmt/format.h>
#include <fstream>
#include <vector>

namespace vb {
    struct tri {
        double x, y, z;
    };

    inline std::ostream &operator<<(std::ostream &os, const tri &c) { return os << "<" << c.x << "," << c.y << "," << c.z << ">"; }

    struct bunch : public std::vector<std::string> {
        bunch(std::string bef = "", std::string aft = "") : before(bef), after(aft) {}
        template <typename T> bunch &operator<<(const T &t) {
            push_back(fmt::format("{}", t));
            return *this;
        }
        std::string before, after;
    };

    std::ostream &operator<<(std::ostream &os, const bunch &b);

    class Pov_Scene : public bunch {
    public:
        Pov_Scene() {
            (*this) << R"(#version 3.7;)"
                    << R"(#include "colors.inc")"
                    << R"(#include "rad_def.inc")"
                    << R"(global_settings { radiosity { Rad_Settings(Radiosity_Normal,off,off) } })"
                    << R"(background { color White })";
        }

        void output_pov(const std::string &s);
    };

    class Pov_Union : public bunch {
    public:
        Pov_Union() : bunch("union {", "}"){};
    };

    std::string Box(tri a, tri b);
    std::string Camera(tri a, tri b, double d);
    std::string Cylinder(tri a, tri b, double r);
    std::string Light_Source(tri a);
    std::string Plane(tri a, double d);
    std::string Sphere(tri a, double r);
    std::string Texture(const std::string &t);
    Pov_Union   Frame(tri a, tri b, const std::string &t);
} // namespace vb
