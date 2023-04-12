#pragma once /// @file
#include <vb/math/Permutation.h>
#include <vb/util/cpx.h>
#ifdef None
#undef None
#endif
#include <yaml-cpp/yaml.h>

namespace vb {
    class Hypermap {
    public:
        struct Vertex {
            size_t              i = 0;
            cpx                 z;
            double              r    = 1.0;
            unsigned            bone = 0;
            std::vector<size_t> adj;
            bool                fixed = false;
        };
        struct Edge {
            size_t i = 0, src = 0;
            double a = NAN;
        };

        Hypermap(Permutation s, Permutation a, Permutation p)
            : sigma(std::move(s)), alpha(std::move(a)), phi(std::move(p)), initial(sigma.size(), 3) {}
        Hypermap(Cycles s, Cycles a, Cycles p) : Hypermap(Permutation(s), Permutation(a), Permutation(p)) {}
        Hypermap() = default;

        void from_hypermap(); //< If the Hypermap part is correct, fill in V and E.

        auto operator==(const Hypermap &o) const -> bool { return (sigma == o.sigma) && (alpha == o.alpha); }

        [[nodiscard]] auto validate() const -> bool;
        [[nodiscard]] auto is_graph() const -> bool;
        [[nodiscard]] auto is_triangulation() const -> bool;
        [[nodiscard]] auto is_simple(size_t d = 2) const -> bool; // ! parallel but non-consecutive edges (like an eye) are not detected
        [[nodiscard]] auto euler() const -> int;
        [[nodiscard]] auto genus() const -> unsigned;

        void flip(size_t e);

        void acpa();

        void relabel(const Permutation &p);

        void normalize();
        void mirror();
        void dual();
        void simplify1();
        void simplify2();
        void simplify(size_t d = 2);
        void split_edges();
        void dessin();

        Permutation sigma, alpha, phi;

        std::vector<unsigned> initial;
        std::vector<Vertex>   V;
        std::vector<Edge>     E;

        static auto alpha_xyz(double x, double y, double z) -> double;
        static auto ccn(size_t n) -> double;

    private:
        [[nodiscard]] auto rebasing(size_t i) const -> Permutation;
        [[nodiscard]] auto rebasing() const -> Permutation;
    };

    auto hypermaps(const Permutation &s, const Permutation &a, const Permutation &p) -> Stream<Hypermap>;
} // namespace vb

namespace YAML {
    template <> struct convert<vb::Hypermap> {
        static auto encode(const vb::Hypermap &h) -> Node;
        static auto decode(const Node &node, vb::Hypermap &h) -> bool;
    };
} // namespace YAML

template <> struct fmt::formatter<vb::Hypermap> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }
    template <typename FormatContext> auto          format(const vb::Hypermap &H, FormatContext &ctx) {
        fmt::format_to(ctx.out(), "Hypermap < {} black, {} white, {} half-edges, {} faces, genus {} >\n", H.sigma.cycles().size(),
                                H.alpha.cycles().size(), H.sigma.size(), H.phi.cycles().size(), H.genus());
        fmt::format_to(ctx.out(), "  sigma: {}\n", H.sigma);
        fmt::format_to(ctx.out(), "  alpha: {}\n", H.alpha);
        return fmt::format_to(ctx.out(), "    phi: {}", H.phi);
        // return os;

        // return fmt::format_to(ctx.out(), "{}", M);
    }
};
