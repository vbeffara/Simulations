#include <fmt/printf.h>
#include <map>
#include <vb/Figure.h>
#include <vb/ProgressBar.h>
#include <vb/data/Array.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>
#include <vb/util/coo.h>

using namespace vb;

// First define the structure of the triangular lattice to make the code below clearer.

struct Pattern {
  size_t                                       n_sites, n_bonds, n_faces;
  cpx                                          period;
  std::vector<cpx>                             shifts;
  std::vector<std::tuple<size_t, size_t, coo>> adj_faces;
  std::vector<std::tuple<size_t, size_t, coo>> face_edges;
  std::vector<std::tuple<size_t, size_t, coo>> face_sites;
  std::vector<std::tuple<size_t, size_t, coo>> edge_sites;

  Pattern(size_t s, size_t b, size_t f) : n_sites(s), n_bonds(b), n_faces(f) {}

  cpx pos(coo z) { return {z.x + period.real() * z.y, period.imag() * z.y}; }

  std::unique_ptr<Segment> draw_edge(coo z, size_t k) {
    std::vector<cpx> pts;
    for (auto [i, j, dz] : edge_sites)
      if (i == k) pts.push_back(pos(z + dz) + shifts[j]);
    return std::make_unique<Segment>(pts[0], pts[1], Pen(BLACK, 1.5));
  }

  std::unique_ptr<Path> draw_face(coo z, long long c, size_t k) {
    Path ans({}, Pen(BLACK, 3.0, Indexed(c), true));
    for (auto [i, j, dz] : face_sites)
      if (i == k) ans.z.push_back(pos(z + dz) + shifts[j]);
    ans.z.push_back(ans.z[0]);
    return std::make_unique<Path>(ans);
  }
};

Pattern Triangular() {
  Pattern P(1, 3, 2); // Order of edges : E, N, NW
  P.period     = {.5, sqrt(3) / 2};
  P.shifts     = {0};
  P.adj_faces  = {{0, 1, {0, 0}}, {0, 1, {0, -1}}, {0, 1, {-1, 0}}, {1, 0, {0, 0}}, {1, 0, {0, 1}}, {1, 0, {1, 0}}};
  P.face_edges = {{0, 0, {0, 0}}, {0, 1, {0, 0}}, {0, 2, {1, 0}}, {1, 0, {0, 1}}, {1, 1, {1, 0}}, {1, 2, {1, 0}}};
  P.face_sites = {{0, 0, {0, 0}}, {0, 0, {1, 0}}, {0, 0, {0, 1}}, {1, 0, {1, 0}}, {1, 0, {1, 1}}, {1, 0, {0, 1}}};
  P.edge_sites = {{0, 0, {0, 0}}, {0, 0, {1, 0}}, {1, 0, {0, 0}}, {1, 0, {0, 1}}, {2, 0, {0, 0}}, {2, 0, {-1, 1}}};
  return P;
}

struct Perco {
  struct Site {
    std::vector<double>    edge_labels, face_labels, reaching_time;
    std::vector<long long> c;
  };

  Pattern             P;
  size_t              n;
  double              p;
  Array<Site>         sites;
  std::vector<size_t> sizes;

  // Initialize edge labels as iid uniforms, and face labels as the max of incident edge labels.
  Perco(Pattern P, size_t n, double p) : P(P), n(n), p(p), sites({n, n}) {
    for (auto z : coo_range<long long>({int(n), int(n)})) {
      Site &s = sites.atp(z);
      for (size_t i = 0; i < P.n_bonds; ++i) s.edge_labels.push_back(prng.uniform_real());
      for (size_t i = 0; i < P.n_faces; ++i) s.face_labels.push_back(0);
      for (size_t i = 0; i < P.n_faces; ++i) s.reaching_time.push_back(1);
      for (size_t i = 0; i < P.n_faces; ++i) s.c.push_back(0);
    }
    for (auto z : coo_range<long long>({int(n), int(n)})) {
      Site &s = sites.atp(z);
      for (auto [i, j, dz] : P.face_edges) s.face_labels[i] = std::max(s.face_labels[i], sites.atp(z + dz).edge_labels[j]);
    }
  }

  void show(double p) {
    Figure F{"Facet Percolation"};
    for (auto z : coo_range<long long>({int(n), int(n)})) {
      Site &s = sites.atp(z);
      for (size_t i = 0; i < P.n_faces; ++i)
        if (s.face_labels[i] < p) F.add(P.draw_face(z, s.c[i], i));
      for (size_t i = 0; i < P.n_bonds; ++i)
        if (s.edge_labels[i] < p) F.add(P.draw_edge(z, i));
      F.add(std::make_unique<Circle>(P.pos(z), .1, Pen(BLACK, 1, BLACK, true)));
    }
    F.show();
    F.pause();
    F.output_pdf("Facet percolation");
  }

  // Count cluster sizes
  void count() {
    std::map<long long, size_t> count;
    for (auto z : coo_range<long long>({int(n), int(n)})) {
      if (sites.atp(z).face_labels[0] < p) ++count[sites.atp(z).c[0]];
      if (sites.atp(z).face_labels[1] < p) ++count[sites.atp(z).c[1]];
    }

    for (auto [c, n] : count) sizes.push_back(n);
    std::sort(sizes.begin(), sizes.end(), std::greater<size_t>());
  }

  void explore() {
    long long idx = 0;
    for (auto z : coo_range<long long>({int(n), int(n)})) {
      if (sites.atp(z).face_labels[0] < p) sites.atp(z).c[0] = ++idx;
      if (sites.atp(z).face_labels[1] < p) sites.atp(z).c[1] = ++idx;
    }

    bool done = false;
    while (!done) {
      done = true;
      for (auto z : coo_range<long long>({int(n), int(n)})) {
        for (auto [i, j, dz] : P.adj_faces) {
          if (sites.atp(z).face_labels[i] < p && sites.atp(z + dz).c[j] > sites.atp(z).c[i]) {
            sites.atp(z).c[i] = sites.atp(z + dz).c[j];
            done              = false;
          }
        }
      }
    }

    count();
  }

  size_t biggest() { return sizes.size() > 0 ? sizes[0] : 0; }

  size_t second_biggest() { return sizes.size() > 1 ? sizes[1] : 0; }

  double compute_reaching_time() {
    std::vector<std::pair<coo, size_t>> stack;
    for (size_t i = 0; i < n; ++i) {
      sites[{0, i}].reaching_time[0] = 0;
      stack.push_back({{0, int(i)}, 0});
    }

    size_t i = 0;
    while (i < stack.size()) {
      auto [z, k] = stack[i++];
      for (auto [i, j, dz] : P.adj_faces) {
        if (i != k) continue;
        double candidate = std::max(sites.atp(z).reaching_time[i], sites.atp(z + dz).face_labels[j]);
        if (candidate < sites.atp(z + dz).reaching_time[j]) {
          sites.atp(z + dz).reaching_time[j] = candidate;
          stack.push_back({z + dz, j});
        }
      }
    }

    double ans = 1;
    for (size_t i = 0; i < n; ++i) ans = std::min(ans, sites[{n / 2, i}].reaching_time[0]);
    return ans;
  }
};

int main(int argc, char **argv) {
  CLP  clp(argc, argv, "Facet percolation");
  auto n = clp.param("n", size_t(50), "Domain size");
  auto p = clp.param("p", 0.5, "Percolation parameter (only used if t=0)");
  auto t = clp.param("t", size_t(0), "Number of samples (for p_c estimation, ignores p)");
  clp.finalize();

  if (t == 0) {
    Perco P(Triangular(), n, p);
    P.explore();
    spdlog::info("Largest cluster size: {}", P.biggest());
    P.show(p);
  } else {
    ProgressBar PB(t);
    for (size_t i = 0; i < t; ++i) {
      PB.set(i);
      Perco P(Triangular(), n, 0);
      fmt::print("{} {}\n", n, P.compute_reaching_time());
    }
  }
}