#pragma once /// \file
#include <fmt/ostream.h>
#include <fstream>
#include <utility>
#include <vector>

namespace vb {
    struct tri {
        double x, y, z;
    };

    struct bunch : public std::vector<std::string> {
        explicit bunch(std::string bef = "", std::string aft = "") : before(std::move(std::move(bef))), after(std::move(std::move(aft))) {}
        template <typename T> auto operator<<(const T &t) -> bunch & {
            push_back(fmt::format("{}", t));
            return *this;
        }
        std::string before, after;
    };

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

    auto Box(tri a, tri b) -> std::string;
    auto Camera(tri a, tri b, double d) -> std::string;
    auto Cylinder(tri a, tri b, double r) -> std::string;
    auto Light_Source(tri a) -> std::string;
    auto Plane(tri a, double d) -> std::string;
    auto Sphere(tri a, double r) -> std::string;
    auto Texture(const std::string &t) -> std::string;
    auto Frame(tri a, tri b, const std::string &t) -> Pov_Union;
} // namespace vb

template <> struct fmt::formatter<vb::tri> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const vb::tri &c, FormatContext &ctx) {
        return format_to(ctx.out(), "<{},{},{}>", c.x, c.y, c.z);
    }
};

template <> struct fmt::formatter<vb::bunch> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const vb::bunch &b, FormatContext &ctx) {
        if (!b.before.empty()) format_to(ctx.out(), "{}", b.before + '\n');
        for (const auto &s : b) format_to(ctx.out(), "{}", s + '\n');
        if (!b.after.empty()) format_to(ctx.out(), "{}", b.after + '\n');
        return ctx.out();
    }
};
