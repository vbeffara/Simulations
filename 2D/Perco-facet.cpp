#include <fmt/printf.h>
#include <map>
#include <vb/Figure.h>
#include <vb/ProgressBar.h>
#include <vb/data/Array.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>
#include <vb/util/coo.h>

using namespace vb;

struct Site {
  bool E{false}, N{false}, NW{false}, T1{false}, T2{false};
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
  size_t              n;
  Array<Site>         sites;
  Array<long long>    c1, c2;
  std::vector<size_t> sizes;

  Perco(size_t n, double p) : n(n), sites({n, n}), c1({n, n}, 0), c2({n, n}, 0) {
    // Open each edge with probability `p`
    for (auto z : coo_range<size_t>({n, n})) {
      if (prng.bernoulli(p)) sites[z].E = true;
      if (prng.bernoulli(p)) sites[z].N = true;
      if (prng.bernoulli(p)) sites[z].NW = true;
    }
    // Open each facet if the corresponding edges are open
    for (auto z : coo_range<long long>({int(n), int(n)})) {
      if (sites.atp(z).E && sites.atp(z).N && sites.atp({z.x + 1, z.y}).NW) sites.atp(z).T1 = true;
      if (sites.atp({z.x + 1, z.y}).NW && sites.atp({z.x + 1, z.y}).N && sites.atp({z.x, z.y + 1}).E) sites.atp(z).T2 = true;
    }
  }

  void show() {
    Figure F{"Percolation"};
    for (auto z : coo_range<long long>({int(n), int(n)})) {
      cpx pos{z.x + double(z.y) / 2, z.y * sqrt(3) / 2};
      F.add(std::make_unique<Circle>(pos, .1));
      if (sites.atp(z).T1) F.add(tri1(z, c1.atp(z)));
      if (sites.atp(z).T2) F.add(tri2(z, c2.atp(z)));
      if (sites.atp(z).E) F.add(seg_E(z));
      if (sites.atp(z).N) F.add(seg_N(z));
      if (sites.atp(z).NW) F.add(seg_NW(z));
    }
    F.show();
    F.pause();
    F.output_pdf("Facet percolation");
  }

  // Count cluster sizes
  void count() {
    std::map<long long, size_t> count;
    for (auto z : coo_range<size_t>({n, n})) {
      if (sites[z].T1) ++count[c1[z]];
      if (sites[z].T2) ++count[c2[z]];
    }

    for (auto [c, n] : count) sizes.push_back(n);
    std::sort(sizes.begin(), sizes.end(), std::greater<size_t>());
  }

  void explore() {
    long long idx = 0;
    for (auto z : coo_range<size_t>({n, n})) {
      if (sites[z].T1) c1[z] = ++idx;
      if (sites[z].T2) c2[z] = ++idx;
    }

    bool done = false;
    while (!done) {
      done = true;
      for (auto z : coo_range<long long>({int(n), int(n)})) {
        if (sites.atp(z).T1) {
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
        if (sites.atp(z).T2) {
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
  auto v = clp.flag("v", "Show the picture");
  clp.finalize();

  if (t == 1) {
    Perco P(n, p);
    P.explore();
    spdlog::info("Largest cluster size: {}", P.biggest());
    P.show();
  } else {
    ProgressBar PB(t);
    for (size_t i = 0; i < t; ++i) {
      PB.set(i);
      double p = prng.uniform_real();
      Perco  P(n, p);
      P.explore();
      fmt::print("{} {} {}\n", p, P.biggest(), P.second_biggest());
    }
  }
}