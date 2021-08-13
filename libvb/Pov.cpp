#include <vb/Pov.h>

namespace vb {
    void Pov_Scene::output_pov(const std::string &s) { std::ofstream((s + ".pov").c_str()) << fmt::format("{}", static_cast<bunch>(*this)); }

    auto Box(tri a, tri b) -> std::string { return fmt::format("box {{ {}, {} }}", a, b); }
    auto Camera(tri a, tri b, double d) -> std::string { return fmt::format("camera {{ location {} look_at {} angle {} }}", a, b, d); }
    auto Cylinder(tri a, tri b, double r) -> std::string { return fmt::format("cylinder {{ {}, {}, {} }}", a, b, r); }
    auto Light_Source(tri a) -> std::string { return fmt::format("light_source {{ {} color White*2 }}", a); }
    auto Plane(tri a, double d) -> std::string { return fmt::format("plane {{ {}, {} }}", a, d); }
    auto Sphere(tri a, double r) -> std::string { return fmt::format("sphere {{ {}, {} }}", a, r); }
    auto Texture(const std::string &t) -> std::string { return fmt::format("texture {{ {} }}", t); }

    auto Frame(tri a, tri b, const std::string &t) -> Pov_Union {
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
