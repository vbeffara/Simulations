#include <spdlog/spdlog.h>
#include <vb/Map.h>
#include <vb/ProgressBar.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using vb::Edge;
using vb::Hub;
using vb::Map;

class Triangulation : public Map {
public:
    Triangulation(const std::string &s, size_t n) : Map(s, n) {
        for (size_t i = 1; i < n; ++i) (*this) << Edge(0, i);
        for (size_t i = n - 1; i > 1; --i) (*this) << Edge(1, i);
        (*this) << Edge(1, 0) << Edge(2, 0) << Edge(2, 1) << Edge(2, 3);
        for (size_t i = 3; i < n - 1; ++i) (*this) << Edge(i, 0) << Edge(i, i - 1) << Edge(i, 1) << Edge(i, i + 1);
        (*this) << Edge(n - 1, 0) << Edge(n - 1, n - 2) << Edge(n - 1, 1);
    }

    [[nodiscard]] auto random_edge() const -> Edge {
        auto                   i = vb::prng.uniform_int(n), j = vb::prng.uniform_int(v[i]->adj.size());
        vb::adj_list::iterator k;
        for (k = v[i]->adj.begin(); j > 0; ++k, --j) {};
        return Edge(i, *k);
    }

    auto flip(Edge e) -> bool {
        auto i = e.first, j = e.second;

        auto e1 = find_edge(Edge(i, j));
        if (e1 == v[i]->adj.begin()) e1 = v[i]->adj.end();
        --e1;
        auto k  = *e1;
        auto e2 = find_edge(Edge(j, i));
        if (e2 == v[j]->adj.begin()) e2 = v[j]->adj.end();
        --e2;
        auto l = *e2;

        if (k == l) return false;
        if (find_edge(Edge(k, l)) != v[0]->adj.end()) return false;

        ++e1;
        if (e1 == v[i]->adj.end()) e1 = v[i]->adj.begin();
        v[i]->adj.erase(e1);
        ++e2;
        if (e2 == v[j]->adj.end()) e2 = v[j]->adj.begin();
        v[j]->adj.erase(e2);

        add_after(Edge(k, j), l);
        add_after(Edge(l, i), k);
        return true;
    }
};

auto main(int argc, char **argv) -> int {
    vb::Hub H("Random triangulation", argc, argv, "n=10,t=0");
    size_t  n = H['n'], t = H['t'];
    if (t == 0) t = 50 * n * n;

    Triangulation T(H.title, H['n']);
    T.inscribe(T.face(Edge(0, 1)));

    {
        vb::ProgressBar P(t);
        for (size_t i = 0; i < t; ++i) {
            T.flip(T.random_edge());
            P.set(i);
        }
    }

    T.show();
    T.inscribe(T.face(Edge(0, *(T.v[0]->adj.begin()))));
    T.balance_old();
    T.pause();
    spdlog::info("{}", static_cast<Map &>(T));
}
