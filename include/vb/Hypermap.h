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

        bool operator==(const Hypermap &o) const { return (sigma == o.sigma) && (alpha == o.alpha); }

        bool     validate() const;
        bool     is_graph() const;
        bool     is_triangulation() const;
        bool     is_simple(size_t d = 2) const; // ! parallel but non-consecutive edges (like an eye) are not detected
        int      euler() const;
        unsigned genus() const;

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

        static double alpha_xyz(double x, double y, double z);
        static double ccn(size_t n);

    private:
        Permutation rebasing(size_t i) const;
        Permutation rebasing() const;
    };

    Stream<Hypermap> hypermaps(const Permutation &s, const Permutation &a, const Permutation &p);
} // namespace vb

namespace YAML {
    template <> struct convert<vb::Hypermap> {
        static Node encode(const vb::Hypermap &h);
        static bool decode(const Node &node, vb::Hypermap &h);
    };
} // namespace YAML

template <> struct fmt::formatter<vb::Hypermap> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }
    template <typename FormatContext> auto          format(const vb::Hypermap &H, FormatContext &ctx) {
        format_to(ctx.out(), "Hypermap < {} black, {} white, {} half-edges, {} faces, genus {} >\n", H.sigma.cycles().size(),
                  H.alpha.cycles().size(), H.sigma.size(), H.phi.cycles().size(), H.genus());
        format_to(ctx.out(), "  sigma: {}\n", H.sigma);
        format_to(ctx.out(), "  alpha: {}\n", H.alpha);
        return format_to(ctx.out(), "    phi: {}", H.phi);
        // return os;

        // return format_to(ctx.out(), "{}", M);
    }
};
