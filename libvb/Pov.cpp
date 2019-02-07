#include <vb/Pov.h>

namespace vb {
    std::ostream &operator<<(std::ostream &os, const bunch &b) {
        if (!b.before.empty()) os << b.before << '\n';
        for (const auto &s : b) os << s << '\n';
        if (!b.after.empty()) os << b.after;
        return os;
    }

    void Pov_Scene::output_pov(const Hub &H, const std::string &s) { std::ofstream((H.dir + s + ".pov").c_str()) << (*this); }

    std::string Box(tri a, tri b) { return fmt::format("box {{ {}, {} }}", a, b); }
    std::string Camera(tri a, tri b, double d) { return fmt::format("camera {{ location {} look_at {} angle {} }}", a, b, d); }
    std::string Cylinder(tri a, tri b, double r) { return fmt::format("cylinder {{ {}, {}, {} }}", a, b, r); }
    std::string Light_Source(tri a) { return fmt::format("light_source {{ {} color White*2 }}", a); }
    std::string Plane(tri a, double d) { return fmt::format("plane {{ {}, {} }}", a, d); }
    std::string Sphere(tri a, double r) { return fmt::format("sphere {{ {}, {} }}", a, r); }
    std::string Texture(const std::string &t) { return fmt::format("texture {{ {} }}", t); }

    Pov_Union Frame(tri a, tri b, const std::string &t) {
        Pov_Union u;
        u << Sphere({a.x, a.y, a.z}, .1) << Sphere({a.x, a.y, b.z}, .1) << Sphere({a.x, b.y, a.z}, .1) << Sphere({a.x, b.y, b.z}, .1)
          << Sphere({b.x, a.y, a.z}, .1) << Sphere({b.x, a.y, b.z}, .1) << Sphere({b.x, b.y, a.z}, .1) << Sphere({b.x, b.y, b.z}, .1)
          << Cylinder({a.x, a.y, a.z}, {a.x, a.y, b.z}, .1) << Cylinder({a.x, a.y, a.z}, {a.x, b.y, a.z}, .1)
          << Cylinder({a.x, a.y, a.z}, {b.x, a.y, a.z}, .1) << Cylinder({a.x, b.y, b.z}, {a.x, a.y, b.z}, .1)
          << Cylinder({a.x, b.y, b.z}, {a.x, b.y, a.z}, .1) << Cylinder({a.x, b.y, b.z}, {b.x, b.y, b.z}, .1)
          << Cylinder({b.x, a.y, b.z}, {a.x, a.y, b.z}, .1) << Cylinder({b.x, a.y, b.z}, {b.x, b.y, b.z}, .1)
          << Cylinder({b.x, a.y, b.z}, {b.x, a.y, a.z}, .1) << Cylinder({b.x, b.y, a.z}, {a.x, b.y, a.z}, .1)
          << Cylinder({b.x, b.y, a.z}, {b.x, a.y, a.z}, .1) << Cylinder({b.x, b.y, a.z}, {b.x, b.y, b.z}, .1) << Texture(t);
        return u;
    }
} // namespace vb
