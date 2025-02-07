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

  Pattern(size_t s, size_t b, size_t f) : n_sites(s), n_bonds(b), n_faces(f) {}

  cpx pos(coo z) { return {z.x + period.real() * z.y, period.imag() * z.y}; }
};

Pattern Triangular() {
  Pattern P(1, 3, 2); // Order of edges : E, N, NW
  P.period     = {.5, sqrt(3) / 2};
  P.shifts     = {0};
  P.adj_faces  = {{0, 1, {0, 0}}, {0, 1, {0, -1}}, {0, 1, {-1, 0}}, {1, 0, {0, 0}}, {1, 0, {0, 1}}, {1, 0, {1, 0}}};
  P.face_edges = {{0, 0, {0, 0}}, {0, 1, {0, 0}}, {0, 2, {1, 0}}, {1, 0, {0, 1}}, {1, 1, {1, 0}}, {1, 2, {1, 0}}};
  return P;
}

// The data in one fundamental domain of the lattice.

struct Site {
  std::vector<double> edge_labels, face_labels;
};

cpx pos(coo z) { return {z.x + double(z.y) / 2, z.y * sqrt(3) / 2}; }

std::unique_ptr<Segment> seg_E(coo z) { return std::make_unique<Segment>(pos(z), pos(z) + cpx(1, 0)); }

std::unique_ptr<Segment> seg_N(coo z) { return std::make_unique<Segment>(pos(z), pos(z) + cpx(.5, sqrt(3) / 2)); }

std::unique_ptr<Segment> seg_NW(coo z) { return std::make_unique<Segment>(pos(z), pos(z) + cpx(-.5, sqrt(3) / 2)); }

std::unique_ptr<Path> tri1(coo z, long long c) {
  Path P({pos(z), pos(z) + cpx(1, 0), pos(z) + cpx(.5, sqrt(3) / 2), pos(z)}, Pen(BLACK, 1.0, Indexed(c), true));
  return std::make_unique<Path>(P);
}

std::unique_ptr<Path> tri2(coo z, long long c) {
  Path P({pos(z) + cpx(1, 0), pos(z) + cpx(1.5, sqrt(3) / 2), pos(z) + cpx(.5, sqrt(3) / 2), pos(z) + cpx(1, 0)},
         Pen(BLACK, 1.0, Indexed(c), true));
  return std::make_unique<Path>(P);
}

struct Perco {
  Pattern             P;
  size_t              n;
  double              p;
  Array<Site>         sites;
  Array<long long>    c1, c2;
  std::vector<size_t> sizes;

  // Initialize edge labels as iid uniforms, and face labels as the max of incident edge labels.
  Perco(Pattern P, size_t n, double p) : P(P), n(n), p(p), sites({n, n}), c1({n, n}, 0), c2({n, n}, 0) {
    for (auto z : coo_range<long long>({int(n), int(n)})) {
      for (size_t i = 0; i < P.n_bonds; ++i) sites.atp(z).edge_labels.push_back(prng.uniform_real());
      for (size_t i = 0; i < P.n_faces; ++i) sites.atp(z).face_labels.push_back(0);
    }

    for (auto z : coo_range<long long>({int(n), int(n)}))
      for (auto [i, j, dz] : P.face_edges)
        if (sites.atp(z + dz).edge_labels[j] > sites.atp(z).face_labels[i]) sites.atp(z).face_labels[i] = sites.atp(z + dz).edge_labels[j];
  }

  void show(double p) {
    Figure F{"Percolation"};
    for (auto z : coo_range<long long>({int(n), int(n)})) {
      cpx pos = P.pos(z);
      F.add(std::make_unique<Circle>(pos, .1));
      if (sites.atp(z).face_labels[0] < p) F.add(tri1(z, c1.atp(z)));
      if (sites.atp(z).face_labels[1] < p) F.add(tri2(z, c2.atp(z)));
      if (sites.atp(z).edge_labels[0] < p) F.add(seg_E(z));
      if (sites.atp(z).edge_labels[1] < p) F.add(seg_N(z));
      if (sites.atp(z).edge_labels[2] < p) F.add(seg_NW(z));
    }
    F.show();
    F.pause();
    F.output_pdf("Facet percolation");
  }

  // Count cluster sizes
  void count() {
    std::map<long long, size_t> count;
    for (auto z : coo_range<long long>({int(n), int(n)})) {
      if (sites.atp(z).face_labels[0] < p) ++count[c1.atp(z)];
      if (sites.atp(z).face_labels[1] < p) ++count[c2.atp(z)];
    }

    for (auto [c, n] : count) sizes.push_back(n);
    std::sort(sizes.begin(), sizes.end(), std::greater<size_t>());
  }

  void explore() {
    long long idx = 0;
    for (auto z : coo_range<long long>({int(n), int(n)})) {
      if (sites.atp(z).face_labels[0] < p) c1.atp(z) = ++idx;
      if (sites.atp(z).face_labels[1] < p) c2.atp(z) = ++idx;
    }

    bool done = false;
    while (!done) {
      done = true;
      for (auto z : coo_range<long long>({int(n), int(n)})) {
        if (sites.atp(z).face_labels[0] < p) {
          if (c2.atp(z) > c1.atp(z)) {
            c1.atp(z) = c2.atp(z);
            done      = false;
          }
          if (c2.atp({z.x - 1, z.y}) > c1.atp(z)) {
            c1.atp(z) = c2.atp({z.x - 1, z.y});
            done      = false;
          }
          if (c2.atp({z.x, z.y - 1}) > c1.atp(z)) {
            c1.atp(z) = c2.atp({z.x, z.y - 1});
            done      = false;
          }
        }
        if (sites.atp(z).face_labels[1] < p) {
          if (c1.atp(z) > c2.atp(z)) {
            c2.atp(z) = c1.atp(z);
            done      = false;
          }
          if (c1.atp({z.x + 1, z.y}) > c2.atp(z)) {
            c2.atp(z) = c1.atp({z.x + 1, z.y});
            done      = false;
          }
          if (c1.atp({z.x, z.y + 1}) > c2.atp(z)) {
            c2.atp(z) = c1.atp({z.x, z.y + 1});
            done      = false;
          }
        }
      }
    }

    count();
  }

  size_t biggest() { return sizes.size() > 0 ? sizes[0] : 0; }

  size_t second_biggest() { return sizes.size() > 1 ? sizes[1] : 0; }
};

int main(int argc, char **argv) {
  CLP  clp(argc, argv, "Facet percolation");
  auto n = clp.param("n", size_t(50), "Domain size");
  auto p = clp.param("p", 0.5, "Percolation parameter (only used if t=1)");
  auto t = clp.param("t", size_t(1), "Number of samples (for p_c estimation, ignores p)");
  clp.finalize();

  if (t == 1) {
    Perco P(Triangular(), n, p);
    P.explore();
    spdlog::info("Largest cluster size: {}", P.biggest());
    P.show(p);
  } else {
    ProgressBar PB(t);
    for (size_t i = 0; i < t; ++i) {
      PB.set(i);
      double p = prng.uniform_real();
      Perco  P(Triangular(), n, p);
      P.explore();
      fmt::print("{} {} {}\n", p, P.biggest(), P.second_biggest());
    }
  }
}