#pragma once
#include <vb/Figure.h>
#include <vb/math/LinearAlgebra.h>

namespace vb {
    using Edge     = std::pair<size_t, size_t>;
    using adj_list = std::vector<size_t>;

    class Vertex : public Circle {
    public:
        adj_list adj;
        explicit Vertex(const cpx &z_ = cpx(0.0, 0.0), const double &r_ = 0.0) : Circle(z_, r_){};
    };

    class Map : public Picture {
    public:
        size_t                               n; // TODO remove after checking that n==v.size() always holds
        std::vector<std::unique_ptr<Vertex>> v;
        std::vector<bool>                    bd;

        double scale = 1;

        Map(const std::string &s, size_t n);

        [[nodiscard]] auto find_edge(const Edge &e) const -> adj_list::iterator;

        [[nodiscard]] auto turn_left(const Edge &e) const -> Edge;
        [[nodiscard]] auto turn_right(const Edge &e) const -> Edge;

        void add_before(const Edge &e, size_t vv);
        void add_after(const Edge &e, size_t vv);

        [[nodiscard]] auto face(Edge e) const -> std::vector<size_t>;

        void inscribe(const std::vector<size_t> &face_ext, const double &radius = 1.0, bool reverse = false);

        auto balance() -> double;
        void balance_old();

        auto split_edges() -> std::vector<size_t>;

        void hex_to_triangle(const std::vector<size_t> &f);
        void barycentric();

        void plot_vertices(Figure *F);
        void plot_edges(Figure *F);
        void plot_circles(Figure *F);

        [[nodiscard]] auto nb_sommets() const -> size_t { return n; }
        auto               nb_aretes() -> size_t;
        auto               nb_faces() -> size_t;
        auto               euler() -> int;
        auto               genre() -> int;

        void mobius(cpx w, const double &theta);

        void rad_to_pos(size_t _zero, size_t _one);

        void rotate(const double &theta);

        void mobius_circle(cpx w, double r);

        auto left() -> double;
        auto right() -> double;
        auto top() -> double;
        auto bottom() -> double;

        auto fg_balance(const Vector<double> &x, Vector<double> *g) -> double;
        auto fg_circle_base(const Vector<double> &x, Vector<double> *g) -> double;
        auto fg_circle_bd(const Vector<double> &x, Vector<double> *g) -> double;
        auto fg_circle_disk(const Vector<double> &x, Vector<double> *g) -> double;

    protected:
        void paint() override;
    };

    auto operator<<(Map &m, const Edge &e) -> Map &;
} // namespace vb

template <> struct fmt::formatter<vb::Map> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const vb::Map &m, FormatContext &ctx) {
        std::vector<std::string> edges;
        for (size_t i = 0; i < m.n; ++i) { edges.push_back(fmt::format("{} -> {}", i, fmt::join(m.v[i]->adj, " "))); }
        format_to(ctx.out(), "{} vertices: {}", m.n, fmt::join(edges, ", "));
        return ctx.out();
    }
};
