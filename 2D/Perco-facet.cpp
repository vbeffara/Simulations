#include <fmt/printf.h>
#include <map>
#include <tbb/parallel_for.h>
#include <vb/Figure.h>
#include <vb/ProgressBar.h>
#include <vb/data/Array.h>
#include <vb/data/Queue.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>
#include <vb/util/coo.h>

using namespace vb;

// First define the structure of the triangular lattice to make the code below clearer.

struct Pattern {
  using Thing = std::pair<coo, size_t>; // Vertex or face or edge

  size_t                          n_sites, n_edges, n_faces;
  cpx                             period;
  std::vector<cpx>                shifts;
  std::vector<std::vector<Thing>> face_to_face, face_to_edge, face_to_site, edge_to_site;

  Pattern(size_t s, size_t b, size_t f) : n_sites(s), n_edges(b), n_faces(f) {}

  cpx pos(coo z) { return {z.x + period.real() * z.y, period.imag() * z.y}; }

  std::unique_ptr<Segment> draw_edge(coo z, size_t k) {
    std::vector<cpx> pts;
    for (auto [dz, j] : edge_to_site[k]) pts.push_back(pos(z + dz) + shifts[j]);
    return std::make_unique<Segment>(pts[0], pts[1], Pen(BLACK, 1.5));
  }

  std::unique_ptr<Path> draw_face(coo z, int64_t c, size_t k) {
    Path ans({}, Pen(BLACK, 3.0, Indexed(c), true));
    for (auto [dz, j] : face_to_site[k]) ans.z.push_back(pos(z + dz) + shifts[j]);
    ans.z.push_back(ans.z[0]);
    return std::make_unique<Path>(ans);
  }
};

Pattern Triangular() {
  Pattern P(1, 3, 2); // Order of edges : E, N, NW
  P.period       = {.5, sqrt(3) / 2};
  P.shifts       = {0};
  P.face_to_face = {{{{0, 0}, 1}, {{0, -1}, 1}, {{-1, 0}, 1}}, {{{0, 0}, 0}, {{0, 1}, 0}, {{1, 0}, 0}}};
  P.face_to_edge = {{{{0, 0}, 0}, {{0, 0}, 1}, {{1, 0}, 2}}, {{{0, 1}, 0}, {{1, 0}, 1}, {{1, 0}, 2}}};
  P.face_to_site = {{{{0, 0}, 0}, {{1, 0}, 0}, {{0, 1}, 0}}, {{{1, 0}, 0}, {{1, 1}, 0}, {{0, 1}, 0}}};
  P.edge_to_site = {{{{0, 0}, 0}, {{1, 0}, 0}}, {{{0, 0}, 0}, {{0, 1}, 0}}, {{{0, 0}, 0}, {{-1, 1}, 0}}};
  return P;
}

struct Perco {
  struct Site {
    std::vector<double>  site_labels, edge_labels, face_labels;
    std::vector<int64_t> c;
    std::vector<bool>    visited;

    Site(const Pattern &P)
        : site_labels(P.n_sites, 0), edge_labels(P.n_edges, 0), face_labels(P.n_faces, 0), c(P.n_faces, 0), visited(P.n_faces, 0) {}
  };

  Pattern     P;
  size_t      n;
  Array<Site> sites;

  double &site_label(coo z, size_t k) { return sites.atp(z).site_labels[k]; }
  double &edge_label(coo z, size_t k) { return sites.atp(z).edge_labels[k]; }
  double &face_label(coo z, size_t k) { return sites.atp(z).face_labels[k]; }

  void fill_from_sites() {
    for (auto z : coo_range<int64_t>({int(n), int(n)}))
      for (size_t i = 0; i < P.n_sites; ++i) site_label(z, i) = prng.uniform_real();
    for (auto z : coo_range<int64_t>({int(n), int(n)}))
      for (size_t i = 0; i < P.n_edges; ++i) {
        edge_label(z, i) = 0;
        for (auto [dz, j] : P.edge_to_site[i]) edge_label(z, i) = std::max(edge_label(z, i), site_label(z + dz, j));
      }
    for (auto z : coo_range<int64_t>({int(n), int(n)})) {
      for (size_t i = 0; i < P.n_faces; ++i) {
        face_label(z, i) = 0;
        for (auto [dz, j] : P.face_to_edge[i]) face_label(z, i) = std::max(face_label(z, i), edge_label(z + dz, j));
      }
    }
  }

  // Initialize edge labels as iid uniforms, and face labels as the max of incident edge labels.
  Perco(Pattern P, size_t n) : P(P), n(n), sites({n, n}, {P}) { fill_from_sites(); }

  void show(double p) {
    Figure F{"Facet Percolation"};
    for (auto z : coo_range<int64_t>({int(n), int(n)})) {
      Site &s = sites.atp(z);
      for (size_t i = 0; i < P.n_faces; ++i)
        if (face_label(z, i) < p) F.add(P.draw_face(z, s.c[i], i));
      for (size_t i = 0; i < P.n_edges; ++i)
        if (edge_label(z, i) < p) F.add(P.draw_edge(z, i));
      F.add(std::make_unique<Circle>(P.pos(z), .1, Pen(BLACK, 1, BLACK, true)));
    }
    F.show();
    F.pause();
    F.output_pdf("Facet percolation");
  }

  void explore(double p) {
    int64_t idx = 0;
    for (auto z : coo_range<int64_t>({int(n), int(n)})) {
      for (size_t i = 0; i < P.n_faces; ++i)
        if (face_label(z, i) < p) sites.atp(z).c[i] = ++idx;
    }

    bool done = false;
    while (!done) {
      done = true;
      for (auto z : coo_range<int64_t>({int(n), int(n)})) {
        for (size_t i = 0; i < P.n_faces; ++i) {
          if (face_label(z, i) > p) continue;
          for (auto [dz, j] : P.face_to_face[i]) {
            if (sites.atp(z + dz).c[j] > sites.atp(z).c[i]) {
              sites.atp(z).c[i] = sites.atp(z + dz).c[j];
              done              = false;
            }
          }
        }
      }
    }
  }

  double compute_reaching_time() {
    Queue<std::pair<coo, size_t>> Q;

    for (int i = 0; i < n; ++i) Q.push({{{0, i}, 0}, 0});

    for (auto z : coo_range<int64_t>({int(n), int(n)})) {
      for (size_t i = 0; i < P.n_faces; ++i) sites.atp(z).visited[i] = false;
    }

    while (!Q.q.empty()) {
      auto [zk, t] = Q.get();
      auto [z, k]  = zk;
      if (pmod(z.x, n) == n / 2) return t;
      Site &s = sites.atp(z);
      if (s.visited[k] == 1) continue;
      s.visited[k] = true;
      for (auto [dz, j] : P.face_to_face[k]) {
        Site &ns = sites.atp(z + dz);
        if (!ns.visited[j]) Q.push({{z + dz, j}, std::max(t, face_label(z + dz, j))});
      }
    }
    return -1; // Should never happen
  }
};

size_t N = 0;

int main(int argc, char **argv) {
  CLP  clp(argc, argv, "Facet percolation");
  auto n = clp.param("n", size_t(50), "Domain size (or max domain size for t>0)");
  auto p = clp.param("p", 0.5, "Percolation parameter (only used if t=0)");
  auto t = clp.param("t", size_t(0), "Number of samples (for p_c estimation, ignores p)");
  clp.finalize();

  if (t == 0) {
    Perco P(Triangular(), n);
    P.explore(p);
    P.show(p);
  } else {
    for (size_t nn = 100; nn <= n; nn *= 2) {
      tbb::parallel_for(size_t(0), t, [nn, t](size_t i) {
        Perco P(Triangular(), nn);
        auto  ans = P.compute_reaching_time();
        fmt::print(std::cerr, "t = {}/{}, n = {} -> {}\n", i, t, nn, ans);
        fmt::print("{} {}\n", nn, ans);
      });
    }
  }
}