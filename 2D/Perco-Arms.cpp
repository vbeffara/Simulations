#define BOOST_NO_CXX98_FUNCTION_BASE
#include <boost/config.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/edmonds_karp_max_flow.hpp>
#include <boost/graph/graph_utility.hpp>
#include <spdlog/spdlog.h>
#include <vb/ProgressBar.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace boost;
using namespace std;
using namespace vb;

using Traits          = adjacency_list_traits<listS, vecS, directedS>;
using edge_descriptor = Traits::edge_descriptor;
using Graph           = adjacency_list<
    listS, vecS, directedS, no_property,
    property<edge_capacity_t, int64_t, property<edge_residual_capacity_t, int64_t, property<edge_reverse_t, edge_descriptor>>>>;

void add_one(Graph *gg, size_t i, size_t j) {
    Graph          &g{*gg};
    edge_descriptor e1, e2;
    bool            t = false;

    tie(e1, t) = add_edge(i, j, g);
    tie(e2, t) = add_edge(j, i, g);

    put(edge_reverse, g, e1, e2);
    put(edge_reverse, g, e2, e1);
}

auto main(int argc, char **argv) -> int {
    Hub    H("Percolation arm exponents", argc, argv, "n=100,t=1,p=.5");
    size_t n = H['n'], n_iter = H['t'];
    double p = H['p'];

    Graph g(n * n + 1);
    for (size_t x = 0; x < n; ++x) {
        for (size_t y = 0; y < n; ++y) {
            auto i = x + n * y;
            if (x < n - 1) add_one(&g, i, i + 1);
            if (y < n - 1) add_one(&g, i, i + n);
        }
    }

    // Wired boundary conditions
    for (size_t i = 0; i < n; ++i) {
        add_one(&g, i, n * n);
        add_one(&g, n * i, n * n);
        add_one(&g, (n - 1) + n * i, n * n);
        add_one(&g, (n - 1) * n + i, n * n);
    }

    // Prepare for quick edge access
    property_map<Graph, edge_capacity_t>::type cap = get(edge_capacity, g);
    Graph::edge_iterator                       e, e_final;

    for (tie(e, e_final) = edges(g); e != e_final; ++e) {
        cap[*e]                       = 0;
        cap[get(edge_reverse, g, *e)] = 0;
    }

    vector<edge_descriptor> all_edges(2 * (n + 1) * n);
    vector<edge_descriptor> rev_edges(2 * (n + 1) * n);

    {
        size_t i = 0;
        for (tie(e, e_final) = edges(g); e != e_final; ++e) {
            if (cap[*e] == 0) {
                all_edges[i]      = *e;
                cap[all_edges[i]] = 1;

                rev_edges[i]      = get(edge_reverse, g, *e);
                cap[rev_edges[i]] = 1;

                ++i;
            }
        }
    }

    vector<int> stats2(5, 0);
    {
        ProgressBar PB(n_iter);
        for (size_t iter = 1; iter <= n_iter; ++iter) {
            for (size_t i = 0; i < 2 * n * (n + 1); ++i) {
                auto o            = prng.bernoulli(p) ? 1 : 0;
                cap[all_edges[i]] = o;
                cap[rev_edges[i]] = o;
            }

            auto flow = edmonds_karp_max_flow(g, (n / 2) * (n + 1), n * n);
            for (size_t i = 0; i <= size_t(flow); ++i) ++stats2[i];

            PB.set(iter);
        }
    }

    vector<string> out2;
    for (unsigned i = 1; i <= 4; ++i) out2.push_back(fmt::format("{} {}", stats2[i], double(stats2[i]) / double(n_iter)));
    spdlog::info("{} | {} | {}", n, n_iter, fmt::join(out2, " | "));
}
