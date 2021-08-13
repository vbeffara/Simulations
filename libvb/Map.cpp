#include <vb/Map.h>
#include <vb/Minimizer.h>

namespace vb {
    Map::Map(const std::string &s, size_t n) : Picture(s, {600, 600}), n(n) {
        for (size_t i = 0; i < n; ++i) {
            v.push_back(std::make_unique<Vertex>(0.0));
            bd.push_back(false);
        }
    }

    // TODO: min_element
    auto Map::left() -> double {
        double l = 0.0;
        for (size_t i = 0; i < n; ++i) l = std::min(l, v[i]->z.real());
        return l;
    }

    auto Map::right() -> double {
        double l = 0.0;
        for (size_t i = 0; i < n; ++i) l = std::max(l, v[i]->z.real());
        return l;
    }

    auto Map::top() -> double {
        double l = 0.0;
        for (size_t i = 0; i < n; ++i) l = std::max(l, v[i]->z.imag());
        return l;
    }

    auto Map::bottom() -> double {
        double l = 0.0;
        for (size_t i = 0; i < n; ++i) l = std::min(l, v[i]->z.imag());
        return l;
    }

    void Map::paint() {
        double width = right() - left(), mid_x = (right() + left()) / 2;
        double height = top() - bottom(), mid_y = (top() + bottom()) / 2;
        double scale_x = w() / width, scale_y = h() / height;

        double local_scale = std::min(scale_x, scale_y);

        cairo_save(cr);
        cairo_set_source_rgb(cr, 1, 1, 1);
        cairo_paint(cr);
        cairo_restore(cr);

        cairo_save(cr);
        cairo_translate(cr, w() * .5, h() * .5);
        cairo_scale(cr, local_scale, local_scale);
        cairo_translate(cr, -mid_x, -mid_y);
        cairo_set_source_rgb(cr, 0, 0, 0);
        cairo_set_line_width(cr, .5 / local_scale);

        for (size_t i = 0; i < n; ++i) {
            for (auto j : v[i]->adj) {
                cairo_move_to(cr, v[i]->z.real(), v[i]->z.imag());
                cairo_line_to(cr, v[j]->z.real(), v[j]->z.imag());
            }
        }

        cairo_stroke(cr);
        cairo_restore(cr);
    }

    auto Map::find_edge(const Edge &e) const -> adj_list::iterator {
        if (e.first >= n) return v[0]->adj.end();
        for (auto i = v[e.first]->adj.begin(); i != v[e.first]->adj.end(); ++i)
            if (*i == e.second) return i;
        return v[0]->adj.end();
    }

    auto Map::turn_left(const Edge &e) const -> Edge {
        auto ee = find_edge(Edge(e.second, e.first));
        if (ee == v[e.second]->adj.begin()) ee = v[e.second]->adj.end();
        --ee;
        return Edge(e.second, *ee);
    }

    auto Map::turn_right(const Edge &e) const -> Edge {
        auto ee = find_edge(Edge(e.second, e.first));
        ++ee;
        if (ee == v[e.second]->adj.end()) ee = v[e.second]->adj.begin();
        return Edge(e.second, *ee);
    }

    void Map::add_before(const Edge &e, size_t vv) {
        auto ee = find_edge(e);
        v[e.first]->adj.insert(ee, vv);
    }

    void Map::add_after(const Edge &e, size_t vv) {
        auto ee = find_edge(e);
        ++ee;
        v[e.first]->adj.insert(ee, vv);
    }

    auto Map::face(Edge e) const -> std::vector<size_t> {
        std::vector<size_t> l;
        auto                first = e.first;
        l.push_back(first);
        e = turn_left(e);
        while (e.first != first) {
            l.push_back(e.first);
            e = turn_left(e);
        }
        return l;
    }

    void Map::inscribe(const std::vector<size_t> &face_ext, const double &radius, bool reverse) {
        auto n_ext = face_ext.size();

        for (size_t i = 0; i < n; ++i) { bd[i] = false; }
        scale = radius;

        int k = 0;
        for (auto i : face_ext) {
            --k;
            bd[i]        = true;
            double angle = (reverse ? -2.0 : 2.0) * 3.1415927 * k / double(n_ext);
            v[i]->z      = radius * cpx(cos(angle), sin(angle));
        }
    }

    auto Map::balance() -> double {
        Vector<double> x(2 * n);

        for (size_t i = 0; i < n; ++i) {
            x[2 * int(i)]     = v[i]->z.real();
            x[2 * int(i) + 1] = v[i]->z.imag();
        }

        Minimizer<double> M(2 * n, [this](const Vector<double> &x, Vector<double> *g) { return fg_balance(x, g); });
        double            output = M.minimize_qn(x);

        for (size_t i = 0; i < n; ++i) { v[i]->z = cpx(M.x[2 * int(i)], M.x[2 * int(i) + 1]); }

        update();
        return output;
    }

    void Map::balance_old() {
        bool dirty = true;

        while (dirty) {
            dirty = false;
            for (size_t i = 0; i < n; ++i) {
                if (bd[i]) continue;
                if (v[i]->adj.empty()) continue;

                cpx old = v[i]->z;
                v[i]->z = 0.0;
                for (auto j : v[i]->adj) v[i]->z += v[j]->z;
                v[i]->z /= double(v[i]->adj.size());
                if (v[i]->z != old) dirty = true;
            }
        }
        update();
    }

    auto Map::split_edges() -> std::vector<size_t> {
        std::vector<int64_t> tmp;
        tmp.reserve(n * n);
        for (size_t i = 0; i < n * n; ++i) tmp.push_back(-1);

        std::vector<size_t>   output;
        std::vector<adj_list> new_vertices;

        for (size_t v1 = 0; v1 < n; ++v1) {
            for (auto v2 = v[v1]->adj.begin(); v2 != v[v1]->adj.end(); ++v2) {
                auto v3 = tmp[v1 + n * (*v2)];
                if (v3 >= 0) {                                                  // If the reverse is already filled
                    (*v2) = size_t(v3);                                         // Then point to it
                } else {                                                        // If it is not
                    adj_list l;                                                 // Create it,
                    l.push_back(v1);                                            // Populate it,
                    l.push_back(*v2);                                           // With its two neighbors,
                    new_vertices.push_back(l);                                  // Add it to the new vertices,
                    v3                  = int64_t(n + new_vertices.size() - 1); // Get its number,
                    tmp[(*v2) + n * v1] = v3;                                   // And mark it for the second way,
                    output.push_back(size_t(v3));                               // Add it to the return list,
                    (*v2) = size_t(v3);                                         // Point to it.
                }
            }
        }

        for (adj_list i : new_vertices) {
            v.push_back(std::make_unique<Vertex>(0.0));
            v.back()->adj = i;
            v.back()->z   = (v[i.front()]->z + v[i.back()]->z) / double(2.0);
            bd.push_back(false);
            ++n;
        }

        return output;
    }

    void Map::hex_to_triangle(const std::vector<size_t> &f) {
        auto i = f.begin();
        auto x = *i;
        ++i;
        auto b = *i;
        ++i;
        auto y = *i;
        ++i;
        auto c = *i;
        ++i;
        auto z = *i;
        ++i;
        auto a = *i;
        ++i;

        add_after(Edge(x, b), y);
        add_after(Edge(y, c), z);
        add_after(Edge(z, a), x);

        add_before(Edge(x, a), z);
        add_before(Edge(y, b), x);
        add_before(Edge(z, c), y);
    }

    void Map::barycentric() {
        auto l = split_edges();
        for (auto i : l) {
            auto f = face(Edge(i, v[i]->adj.front()));
            if (f.size() == 6) hex_to_triangle(f);

            f = face(Edge(i, v[i]->adj.back()));
            if (f.size() == 6) hex_to_triangle(f);
        }
    }

    void Map::plot_vertices(Figure *F) {
        for (size_t i = 0; i < n; ++i) { F->add(std::make_unique<Dot>(v[i]->z)); }
    }

    void Map::plot_edges(Figure *F) {
        for (size_t i = 0; i < n; ++i) {
            for (auto j : v[i]->adj)
                if ((i < j) || (find_edge(Edge(j, i)) == v[0]->adj.end())) F->add(std::make_unique<Segment>(v[i]->z, v[j]->z));
        }
    }

    void Map::plot_circles(Figure *F) {
        for (size_t i = 0; i < n; ++i) {
            if (v[i]->r > 0) F->add(std::make_unique<Circle>(v[i]->z, v[i]->r));
        }
    }

    auto Map::nb_aretes() -> size_t {
        size_t tmp = 0;
        for (size_t i = 0; i < n; ++i) tmp += v[i]->adj.size();
        return (tmp / 2);
    }

    auto Map::nb_faces() -> size_t {
        double tmp = 0.0;
        for (size_t i = 0; i < n; ++i)
            for (auto j : v[i]->adj) tmp += double(1.0) / double(face(Edge(i, j)).size());
        return size_t(tmp + .1);
    }

    auto Map::euler() -> int { return int(nb_sommets()) - int(nb_aretes()) + int(nb_faces()); }

    auto Map::genre() -> int { return 1 - (euler() / 2); }

    void Map::mobius(cpx w, const double &theta) {
        for (size_t i = 0; i < n; ++i) {
            cpx z = v[i]->z / scale;
            w /= scale;
            v[i]->z = scale * (z - w) * cpx(cos(theta), sin(theta)) / (cpx(1, 0) - z * conj(w));
        }
    }

    void Map::rad_to_pos(size_t zero, size_t one) {
        double R_max = 0.0;
        for (size_t i = 0; i < n; ++i)
            if (v[i]->r > R_max) R_max = v[i]->r;

        cpx nowhere(-2 * int(n) * R_max, 0.0);
        for (size_t i = 0; i < n; ++i) v[i]->z = nowhere;

        v[zero]->z = cpx(0.0, 0.0);
        v[one]->z  = cpx(v[zero]->r + v[one]->r, 0.0);

        bool dirty = true;
        while (dirty) {
            dirty = false;

            for (size_t i = 0; i < n; ++i) {
                if (v[i]->z == nowhere) continue;
                if (v[i]->adj.empty()) continue;

                cpx    prev_z = v[v[i]->adj.back()]->z;
                double prev_r = v[v[i]->adj.back()]->r;

                for (auto j : v[i]->adj) {
                    cpx z = v[j]->z;
                    if ((prev_z != nowhere) && (z == nowhere) && ((!bd[i]) || (!bd[j]))) {
                        double x = v[i]->r;
                        double y = v[j]->r;
                        double t = (((x + y) * (x + y) + (x + prev_r) * (x + prev_r) - (y + prev_r) * (y + prev_r)) /
                                    (2 * (x + y) * (x + prev_r)));
                        if (t < -1.0) t = -1.0;
                        if (t > 1.0) t = 1.0;
                        double alpha = acos(t);
                        alpha += arg(prev_z - v[i]->z);
                        v[j]->z = v[i]->z + cpx((x + y) * cos(alpha), (x + y) * sin(alpha));
                        z       = v[j]->z;
                        dirty   = true;
                    }
                    prev_z = z;
                    prev_r = v[j]->r;
                }
            }
        }
    }

    void Map::rotate(const double &theta) {
        for (size_t i = 0; i < n; ++i) v[i]->z *= cpx(cos(theta), sin(theta));
    }

    void Map::mobius_circle(cpx w, double r) {
        w /= scale;
        r /= scale;

        cpx Delta((1 + norm(w) - r * r) * (1 + norm(w) - r * r) - 4 * norm(w), 0);
        cpx x = (cpx(1 + norm(w) - r * r) + sqrt(Delta)) / (cpx(2) * conj(w));
        if (norm(x) > 1) x = (cpx(1 + norm(w) - r * r) - sqrt(Delta)) / (cpx(2) * conj(w));

        for (size_t i = 0; i < n; ++i) {
            cpx    W = v[i]->z / scale;
            double R = v[i]->r / scale;

            cpx A = norm(double(1) - W * conj(x)) - cpx(R * R * norm(x), 0);
            cpx B = (cpx(1, 0) - W * conj(x)) * (conj(x) - conj(W)) - cpx(R * R, 0) * conj(x);
            cpx C = (x - W) * (conj(x) - conj(W)) - cpx(R * R, 0);

            v[i]->z = scale * conj(-B / A);
            v[i]->r = scale * sqrt(abs(norm(v[i]->z / scale) - C / A));
        }
    }

    auto operator<<(Map &m, const Edge &e) -> Map & {
        m.v[e.first]->adj.push_back(e.second);
        return m;
    }

    auto Map::fg_balance(const Vector<double> &x, Vector<double> *g) -> double {
        double c = 0.0;

        for (size_t i = 0; i < n; ++i) {
            (*g)[2 * int(i)]     = 0;
            (*g)[2 * int(i) + 1] = 0;

            for (auto j : v[i]->adj) {
                c += (x[2 * int(j)] - x[2 * int(i)]) * (x[2 * int(j)] - x[2 * int(i)]);
                c += (x[2 * int(j) + 1] - x[2 * int(i) + 1]) * (x[2 * int(j) + 1] - x[2 * int(i) + 1]);
                if (!(bd[i])) {
                    (*g)[2 * int(i)] -= x[2 * int(j)] - x[2 * int(i)];
                    (*g)[2 * int(i) + 1] -= x[2 * int(j) + 1] - x[2 * int(i) + 1];
                }
            }
        }

        return c;
    }

    auto Map::fg_circle_base(const Vector<double> &x, Vector<double> *g) -> double {
        double c = 0.0;

        *g = Vector<double>::Zero(g->rows(), 1);

        for (size_t i = 0; i < n; ++i) {
            for (size_t j : v[i]->adj) {
                double dx   = x[3 * int(j)] - x[3 * int(i)];
                double dy   = x[3 * int(j) + 1] - x[3 * int(i) + 1];
                double l    = sqrt(dx * dx + dy * dy);
                double sr   = x[3 * int(i) + 2] + x[3 * int(j) + 2];
                double lsr  = l - sr;
                double lsrl = lsr / l;

                c += lsr * lsr;

                (*g)[3 * int(i)] -= lsrl * dx;
                (*g)[3 * int(i) + 1] -= lsrl * dy;
                (*g)[3 * int(i) + 2] -= lsr;
            }
        }

        return c;
    }

    auto Map::fg_circle_bd(const Vector<double> &x, Vector<double> *g) -> double {
        double c = fg_circle_base(x, g);

        for (size_t i = 0; i < n; ++i)
            if (bd[i]) (*g)[3 * int(i) + 2] = 0.0;

        return c;
    }

    auto Map::fg_circle_disk(const Vector<double> &x, Vector<double> *g) -> double {
        double c = fg_circle_base(x, g);

        for (int i = 0; i < int(n); ++i) {
            double l2 = x[3 * i] * x[3 * i] + x[3 * i + 1] * x[3 * i + 1];
            double l  = sqrt(l2);
            double r  = x[3 * i + 2];

            if ((bd[unsigned(i)]) || (l + r > 1.0)) {
                double lr1  = l + r - 1.0;
                double lr1r = lr1 / l;

                c += .1 * lr1 * lr1;

                (*g)[3 * i] += .1 * lr1r * x[3 * i];
                (*g)[3 * i + 1] += .1 * lr1r * x[3 * i + 1];
                (*g)[3 * i + 2] += .1 * lr1;
            }
        }

        return c;
    }
} // namespace vb
