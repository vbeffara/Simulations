#include <vb/Map.h>
#include <vb/Minimizer.h>

namespace vb {
    Map::Map(const std::string &s, size_t n_) : Picture(s, {600, 600}), n(n_) {
        for (size_t ii = 0; ii < n; ++ii) {
            v.push_back(std::make_unique<Vertex>(0.0));
            bd.push_back(false);
        }
    }

    // TODO: min_element
    auto Map::left() -> double {
        double l = 0.0;
        for (size_t ii = 0; ii < n; ++ii) l = std::min(l, v[ii]->z.real());
        return l;
    }

    auto Map::right() -> double {
        double l = 0.0;
        for (size_t ii = 0; ii < n; ++ii) l = std::max(l, v[ii]->z.real());
        return l;
    }

    auto Map::top() -> double {
        double l = 0.0;
        for (size_t ii = 0; ii < n; ++ii) l = std::max(l, v[ii]->z.imag());
        return l;
    }

    auto Map::bottom() -> double {
        double l = 0.0;
        for (size_t ii = 0; ii < n; ++ii) l = std::min(l, v[ii]->z.imag());
        return l;
    }

    void Map::paint() {
        double width = right() - left(), mid_x = (right() + left()) / 2;
        double height = top() - bottom(), mid_y = (top() + bottom()) / 2;
        double scale_x = w() / width, scale_y = h() / height;

        double const local_scale = std::min(scale_x, scale_y);

        cairo_save(context);
        cairo_set_source_rgb(context, 1, 1, 1);
        cairo_paint(context);
        cairo_restore(context);

        cairo_save(context);
        cairo_translate(context, w() * .5, h() * .5);
        cairo_scale(context, local_scale, local_scale);
        cairo_translate(context, -mid_x, -mid_y);
        cairo_set_source_rgb(context, 0, 0, 0);
        cairo_set_line_width(context, .5 / local_scale);

        for (size_t ii = 0; ii < n; ++ii) {
            for (auto j : v[ii]->adj) {
                cairo_move_to(context, v[ii]->z.real(), v[ii]->z.imag());
                cairo_line_to(context, v[j]->z.real(), v[j]->z.imag());
            }
        }

        cairo_stroke(context);
        cairo_restore(context);
    }

    auto Map::find_edge(const Edge &e) const -> adj_list::iterator {
        if (e.first >= n) return v[0]->adj.end();
        for (auto ii = v[e.first]->adj.begin(); ii != v[e.first]->adj.end(); ++ii) // TODO range for
            if (*ii == e.second) return ii;
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

        for (size_t ii = 0; ii < n; ++ii) { bd[ii] = false; }
        scale = radius;

        int k = 0;
        for (auto ii : face_ext) {
            --k;
            bd[ii]       = true;
            double const angle = (reverse ? -2.0 : 2.0) * 3.1415927 * k / double(n_ext);
            v[ii]->z     = radius * cpx(cos(angle), sin(angle));
        }
    }

    auto Map::balance() -> double {
        Vector<double> x(2 * n);

        for (size_t ii = 0; ii < n; ++ii) {
            x[2 * int(ii)]     = v[ii]->z.real();
            x[2 * int(ii) + 1] = v[ii]->z.imag();
        }

        Minimizer<double> M(2 * n, [this](const Vector<double> &xx, Vector<double> *gg) { return fg_balance(xx, gg); });
        double const      output = M.minimize_qn(x);

        for (size_t ii = 0; ii < n; ++ii) { v[ii]->z = cpx(M.x[2 * int(ii)], M.x[2 * int(ii) + 1]); }

        update();
        return output;
    }

    void Map::balance_old() {
        bool dirty = true;

        while (dirty) {
            dirty = false;
            for (size_t ii = 0; ii < n; ++ii) {
                if (bd[ii]) continue;
                if (v[ii]->adj.empty()) continue;

                cpx const old = v[ii]->z;
                v[ii]->z = 0.0;
                for (auto j : v[ii]->adj) v[ii]->z += v[j]->z;
                v[ii]->z /= double(v[ii]->adj.size());
                if (v[ii]->z != old) dirty = true;
            }
        }
        update();
    }

    auto Map::split_edges() -> std::vector<size_t> {
        std::vector<int64_t> tmp;
        tmp.reserve(n * n);
        for (size_t ii = 0; ii < n * n; ++ii) tmp.push_back(-1);

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

        for (const auto &l : new_vertices) {
            v.push_back(std::make_unique<Vertex>(0.0));
            v.back()->adj = l;
            v.back()->z   = (v[l.front()]->z + v[l.back()]->z) / double(2.0);
            bd.push_back(false);
            ++n;
        }

        return output;
    }

    void Map::hex_to_triangle(const std::vector<size_t> &f) {
        auto ii = f.begin();
        auto x  = *ii;
        ++ii;
        auto b = *ii;
        ++ii;
        auto y = *ii;
        ++ii;
        auto c = *ii;
        ++ii;
        auto z = *ii;
        ++ii;
        auto a = *ii;
        ++ii;

        add_after(Edge(x, b), y);
        add_after(Edge(y, c), z);
        add_after(Edge(z, a), x);

        add_before(Edge(x, a), z);
        add_before(Edge(y, b), x);
        add_before(Edge(z, c), y);
    }

    void Map::barycentric() {
        auto l = split_edges();
        for (auto ii : l) {
            auto f = face(Edge(ii, v[ii]->adj.front()));
            if (f.size() == 6) hex_to_triangle(f);

            f = face(Edge(ii, v[ii]->adj.back()));
            if (f.size() == 6) hex_to_triangle(f);
        }
    }

    void Map::plot_vertices(Figure *F) {
        for (size_t ii = 0; ii < n; ++ii) { F->add(std::make_unique<Dot>(v[ii]->z)); }
    }

    void Map::plot_edges(Figure *F) {
        for (size_t ii = 0; ii < n; ++ii) {
            for (auto j : v[ii]->adj)
                if ((ii < j) || (find_edge(Edge(j, ii)) == v[0]->adj.end())) F->add(std::make_unique<Segment>(v[ii]->z, v[j]->z));
        }
    }

    void Map::plot_circles(Figure *F) {
        for (size_t ii = 0; ii < n; ++ii) {
            if (v[ii]->r > 0) F->add(std::make_unique<Circle>(v[ii]->z, v[ii]->r));
        }
    }

    auto Map::nb_aretes() -> size_t {
        size_t tmp = 0;
        for (size_t ii = 0; ii < n; ++ii) tmp += v[ii]->adj.size();
        return (tmp / 2);
    }

    auto Map::nb_faces() -> size_t {
        double tmp = 0.0;
        for (size_t ii = 0; ii < n; ++ii)
            for (auto j : v[ii]->adj) tmp += double(1.0) / double(face(Edge(ii, j)).size());
        return size_t(tmp + .1);
    }

    auto Map::euler() -> int { return int(nb_sommets()) - int(nb_aretes()) + int(nb_faces()); }

    auto Map::genre() -> int { return 1 - (euler() / 2); }

    void Map::mobius(cpx w, const double &theta) {
        for (size_t ii = 0; ii < n; ++ii) {
            cpx const z = v[ii]->z / scale;
            w /= scale;
            v[ii]->z = scale * (z - w) * cpx(cos(theta), sin(theta)) / (cpx(1, 0) - z * conj(w));
        }
    }

    void Map::rad_to_pos(size_t zero, size_t one) {
        double R_max = 0.0;
        for (size_t ii = 0; ii < n; ++ii)
            if (v[ii]->r > R_max) R_max = v[ii]->r;

        cpx const nowhere(-2 * int(n) * R_max, 0.0);
        for (size_t ii = 0; ii < n; ++ii) v[ii]->z = nowhere;

        v[zero]->z = cpx(0.0, 0.0);
        v[one]->z  = cpx(v[zero]->r + v[one]->r, 0.0);

        bool dirty = true;
        while (dirty) {
            dirty = false;

            for (size_t ii = 0; ii < n; ++ii) {
                if (v[ii]->z == nowhere) continue;
                if (v[ii]->adj.empty()) continue;

                cpx    prev_z = v[v[ii]->adj.back()]->z;
                double prev_r = v[v[ii]->adj.back()]->r;

                for (auto j : v[ii]->adj) {
                    cpx z = v[j]->z;
                    if ((prev_z != nowhere) && (z == nowhere) && ((!bd[ii]) || (!bd[j]))) {
                        double const x = v[ii]->r;
                        double const y = v[j]->r;
                        double t = (((x + y) * (x + y) + (x + prev_r) * (x + prev_r) - (y + prev_r) * (y + prev_r)) /
                                    (2 * (x + y) * (x + prev_r)));
                        if (t < -1.0) t = -1.0;
                        if (t > 1.0) t = 1.0;
                        double alpha = acos(t);
                        alpha += arg(prev_z - v[ii]->z);
                        v[j]->z = v[ii]->z + cpx((x + y) * cos(alpha), (x + y) * sin(alpha));
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
        for (size_t ii = 0; ii < n; ++ii) v[ii]->z *= cpx(cos(theta), sin(theta));
    }

    void Map::mobius_circle(cpx w, double r) {
        w /= scale;
        r /= scale;

        cpx const Delta((1 + norm(w) - r * r) * (1 + norm(w) - r * r) - 4 * norm(w), 0);
        cpx x = (cpx(1 + norm(w) - r * r) + sqrt(Delta)) / (cpx(2) * conj(w));
        if (norm(x) > 1) x = (cpx(1 + norm(w) - r * r) - sqrt(Delta)) / (cpx(2) * conj(w));

        for (size_t ii = 0; ii < n; ++ii) {
            cpx const    W = v[ii]->z / scale;
            double const R = v[ii]->r / scale;

            cpx const A = norm(double(1) - W * conj(x)) - cpx(R * R * norm(x), 0);
            cpx const B = (cpx(1, 0) - W * conj(x)) * (conj(x) - conj(W)) - cpx(R * R, 0) * conj(x);
            cpx const C = (x - W) * (conj(x) - conj(W)) - cpx(R * R, 0);

            v[ii]->z = scale * conj(-B / A);
            v[ii]->r = scale * sqrt(abs(norm(v[ii]->z / scale) - C / A));
        }
    }

    auto operator<<(Map &m, const Edge &e) -> Map & {
        m.v[e.first]->adj.push_back(e.second);
        return m;
    }

    auto Map::fg_balance(const Vector<double> &x, Vector<double> *gg) -> double {
        double c = 0.0;

        for (size_t ii = 0; ii < n; ++ii) {
            (*gg)[2 * int(ii)]     = 0;
            (*gg)[2 * int(ii) + 1] = 0;

            for (auto j : v[ii]->adj) {
                c += (x[2 * int(j)] - x[2 * int(ii)]) * (x[2 * int(j)] - x[2 * int(ii)]);
                c += (x[2 * int(j) + 1] - x[2 * int(ii) + 1]) * (x[2 * int(j) + 1] - x[2 * int(ii) + 1]);
                if (!(bd[ii])) {
                    (*gg)[2 * int(ii)] -= x[2 * int(j)] - x[2 * int(ii)];
                    (*gg)[2 * int(ii) + 1] -= x[2 * int(j) + 1] - x[2 * int(ii) + 1];
                }
            }
        }

        return c;
    }

    auto Map::fg_circle_base(const Vector<double> &x, Vector<double> *gg) -> double {
        double c = 0.0;

        *gg = Vector<double>::Zero(gg->rows(), 1);

        for (size_t ii = 0; ii < n; ++ii) {
            for (size_t const j : v[ii]->adj) {
                double const dx   = x[3 * int(j)] - x[3 * int(ii)];
                double const dy   = x[3 * int(j) + 1] - x[3 * int(ii) + 1];
                double const l    = sqrt(dx * dx + dy * dy);
                double const sr   = x[3 * int(ii) + 2] + x[3 * int(j) + 2];
                double const lsr  = l - sr;
                double const lsrl = lsr / l;

                c += lsr * lsr;

                (*gg)[3 * int(ii)] -= lsrl * dx;
                (*gg)[3 * int(ii) + 1] -= lsrl * dy;
                (*gg)[3 * int(ii) + 2] -= lsr;
            }
        }

        return c;
    }

    auto Map::fg_circle_bd(const Vector<double> &x, Vector<double> *gg) -> double {
        double const c = fg_circle_base(x, gg);

        for (size_t ii = 0; ii < n; ++ii)
            if (bd[ii]) (*gg)[3 * int(ii) + 2] = 0.0;

        return c;
    }

    auto Map::fg_circle_disk(const Vector<double> &x, Vector<double> *gg) -> double {
        double c = fg_circle_base(x, gg);

        for (int ii = 0; ii < int(n); ++ii) {
            double const l2 = x[3 * ii] * x[3 * ii] + x[3 * ii + 1] * x[3 * ii + 1];
            double const l  = sqrt(l2);
            double const r  = x[3 * ii + 2];

            if ((bd[unsigned(ii)]) || (l + r > 1.0)) {
                double const lr1  = l + r - 1.0;
                double const lr1r = lr1 / l;

                c += .1 * lr1 * lr1;

                (*gg)[3 * ii] += .1 * lr1r * x[3 * ii];
                (*gg)[3 * ii + 1] += .1 * lr1r * x[3 * ii + 1];
                (*gg)[3 * ii + 2] += .1 * lr1;
            }
        }

        return c;
    }
} // namespace vb
