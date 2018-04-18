#pragma once /// \file
#include <iostream>
#include <memory>
#include <variant>
#include <vector>

namespace vb {
    namespace pov {
        struct tri {
            double x, y, z;
        };
        inline std::ostream & operator<<(std::ostream & os, const tri & c) { return os << "<" << c.x << "," << c.y << "," << c.z << ">"; }

        using obj = std::variant<double, tri, std::string>;

        struct output_pov {
            output_pov(std::ostream & os) : os(os) {}
            template <typename T> std::ostream & operator()(T && t) { return os << t; }
            std::ostream &                       os;
        };
        inline std::ostream & operator<<(std::ostream & os, const obj & c) { return std::visit(output_pov(os), c); }
    } // namespace pov

    using pov::tri;

    class Pov_Object {
    public:
        Pov_Object(std::string s, bool b = false);
        virtual ~Pov_Object() = default;

        virtual std::ostream & output_pov(std::ostream & os);
        void                   output_pov(const std::string & s);

        Pov_Object & operator<<(tri a);
        Pov_Object & operator<<(double x);

        template <typename T> Pov_Object & operator<<(std::unique_ptr<T> && p) {
            subs.push_back(std::move(p));
            return *this;
        }

        std::string                              type;
        bool                                     braces;
        int                                      commas;
        std::vector<std::unique_ptr<Pov_Object>> subs;
        std::vector<pov::obj>                    objs;
    };
    inline std::ostream & operator<<(std::ostream & os, Pov_Object * o) { return o->output_pov(os); }

    class Pov_Scene : public Pov_Object {
    public:
        Pov_Scene();
    };
    class Pov_Camera : public Pov_Object {
    public:
        Pov_Camera(tri a, tri b, double d);
    };
    class Pov_Light_Source : public Pov_Object {
    public:
        Pov_Light_Source(tri a);
    };
    class Pov_Union : public Pov_Object {
    public:
        Pov_Union();
    };
    class Pov_Sphere : public Pov_Object {
    public:
        Pov_Sphere(tri a, double r);
    };
    class Pov_Cylinder : public Pov_Object {
    public:
        Pov_Cylinder(tri a, tri b, double r);
    };
    class Pov_Box : public Pov_Object {
    public:
        Pov_Box(tri a, tri b);
    };
    class Pov_Plane : public Pov_Object {
    public:
        Pov_Plane(tri a, double d);
    };
    class Pov_Frame : public Pov_Union {
    public:
        Pov_Frame(tri a, tri b, std::string t = "");
    };

    class Pov_Obj : public Pov_Object {
    public:
        Pov_Obj(pov::obj o) : Pov_Object("unused", false), o(o) {}
        std::ostream & output_pov(std::ostream & os) override { return os << o; }
        pov::obj       o;
    };

    namespace pov {
        inline obj Texture(std::string t) { return "texture { " + t + " }"; }
    } // namespace pov

#ifdef UNIT_TESTS
    TEST_CASE("vb::Pov and friends") { Pov_Box PB({0.0, 1.0, 2.0}, {3.0, 4.0, 5.0}); }
#endif
} // namespace vb
