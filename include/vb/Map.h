#pragma once
#include <vb/Figure.h>
#include <vb/math/LinearAlgebra.h>

namespace vb {
    using Edge     = std::pair<size_t, size_t>;
    using adj_list = std::vector<size_t>;

    class Vertex : public Circle {
    public:
        adj_list adj;
        explicit Vertex(const cpx &z = cpx(0.0, 0.0), const double &r = 0.0) : Circle(z, r){};
    };

    class Map : public Picture {
    public:
        size_t                               n;
        std::vector<std::unique_ptr<Vertex>> v;
        std::vector<bool>                    bd;

        double scale = 1;

        Map(const std::string &s, size_t n);

        adj_list::iterator find_edge(const Edge &e) const;

        Edge turn_left(const Edge &e) const;
        Edge turn_right(const Edge &e) const;

        void add_before(const Edge &e, size_t vv);
        void add_after(const Edge &e, size_t vv);

        std::vector<size_t> face(Edge e);

        void inscribe(const std::vector<size_t> &face_ext, const double &radius = 1.0, bool reverse = false);

        double balance();
        void   balance_old();

        std::vector<size_t> split_edges();

        void hex_to_triangle(const std::vector<size_t> &f);
        void barycentric();

        void plot_vertices(Figure *F);
        void plot_edges(Figure *F);
        void plot_circles(Figure *F);

        size_t nb_sommets() { return n; }
        size_t nb_aretes();
        size_t nb_faces();
        int    euler();
        int    genre();

        void mobius(cpx w, const double &theta);

        void rad_to_pos(size_t _zero, size_t _one);

        void rotate(const double &theta);

        void mobius_circle(cpx w, double r);

        double left();
        double right();
        double top();
        double bottom();

        double fg_balance(const Vector<double> &x, Vector<double> *g);
        double fg_circle_base(const Vector<double> &x, Vector<double> *g);
        double fg_circle_bd(const Vector<double> &x, Vector<double> *g);
        double fg_circle_disk(const Vector<double> &x, Vector<double> *g);

    protected:
        void paint() override;
    };

    Map &operator<<(Map &m, const Edge &e);
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
