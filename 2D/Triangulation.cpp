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
    Triangulation(const std::string &s, size_t nn) : Map(s, nn) {
        for (size_t ii = 1; ii < nn; ++ii) (*this) << Edge(0, ii);
        for (size_t ii = nn - 1; ii > 1; --ii) (*this) << Edge(1, ii);
        (*this) << Edge(1, 0) << Edge(2, 0) << Edge(2, 1) << Edge(2, 3);
        for (size_t ii = 3; ii < nn - 1; ++ii) (*this) << Edge(ii, 0) << Edge(ii, ii - 1) << Edge(ii, 1) << Edge(ii, ii + 1);
        (*this) << Edge(nn - 1, 0) << Edge(nn - 1, nn - 2) << Edge(nn - 1, 1);
    }

    [[nodiscard]] auto random_edge() const -> Edge {
        auto                   ii = vb::prng.uniform_int(n), jj = vb::prng.uniform_int(v[ii]->adj.size());
        vb::adj_list::iterator k;
        for (k = v[ii]->adj.begin(); jj > 0; ++k, --jj) {};
        return Edge(ii, *k);
    }

    auto flip(Edge e) -> bool {
        auto ii = e.first, jj = e.second;

        auto e1 = find_edge(Edge(ii, jj));
        if (e1 == v[ii]->adj.begin()) e1 = v[ii]->adj.end();
        --e1;
        auto k  = *e1;
        auto e2 = find_edge(Edge(jj, ii));
        if (e2 == v[jj]->adj.begin()) e2 = v[jj]->adj.end();
        --e2;
        auto l = *e2;

        if (k == l) return false;
        if (find_edge(Edge(k, l)) != v[0]->adj.end()) return false;

        ++e1;
        if (e1 == v[ii]->adj.end()) e1 = v[ii]->adj.begin();
        v[ii]->adj.erase(e1);
        ++e2;
        if (e2 == v[jj]->adj.end()) e2 = v[jj]->adj.begin();
        v[jj]->adj.erase(e2);

        add_after(Edge(k, jj), l);
        add_after(Edge(l, ii), k);
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
