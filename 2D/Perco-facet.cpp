#include <vb/util/CLP.h>
#include <vb/data/Array.h>
#include <vb/util/PRNG.h>
#include <vb/util/coo.h>
#include <vb/Figure.h>

using namespace vb;

struct Site {
  bool E{false}, N{false}, NW{false};
};

cpx pos (coo z) {
  return {z.x + double(z.y)/2, z.y*sqrt(3)/2};
}

std::unique_ptr<Segment> seg_E (coo z) {
  return std::make_unique<Segment>(pos(z), pos(z) + cpx(1,0));
}

std::unique_ptr<Segment> seg_N (coo z) {
  return std::make_unique<Segment>(pos(z), pos(z) + cpx(.5,sqrt(3)/2));
}

std::unique_ptr<Segment> seg_NW (coo z) {
  return std::make_unique<Segment>(pos(z), pos(z) + cpx(-.5,sqrt(3)/2));
}

std::unique_ptr<Path> tri1 (coo z) {
  Path P({pos(z), pos(z) + cpx(1,0), pos(z) + cpx(.5,sqrt(3)/2), pos(z)}, Pen(BLACK, 1.0, RED, true));
  return std::make_unique<Path>(P);
}

std::unique_ptr<Path> tri2 (coo z) {
  Path P({pos(z) + cpx(1,0), pos(z) + cpx(1.5,sqrt(3)/2), pos(z) + cpx(.5,sqrt(3)/2), pos(z) + cpx(1,0)}, Pen(BLACK, 1.0, RED, true));
  return std::make_unique<Path>(P);
}

struct Perco {
  size_t n;
  Array <Site> sites;

  Perco (size_t n, double p) : n(n), sites({n,n}) {
    for (auto z : coo_range<size_t>({n,n})) {
      if (prng.bernoulli(p)) sites[z].E = true;
      if (prng.bernoulli(p)) sites[z].N = true;
      if (prng.bernoulli(p)) sites[z].NW = true;
    }
  }

  void show() {
    Figure F{"Percolation"};
    for (auto z : coo_range<long long>({int(n),int(n)})) {
      cpx pos {z.x + double(z.y)/2, z.y*sqrt(3)/2};
      F.add(std::make_unique<Circle>(pos, .1));
      if (sites.atp(z).E && sites.atp(z).N && sites.atp({z.x + 1, z.y}).NW)
        F.add(tri1(z));
      if (sites.atp({z.x + 1, z.y}).NW && sites.atp({z.x + 1, z.y}).N && sites.atp({z.x, z.y + 1}).E)
        F.add(tri2(z));
      if (sites.atp(z).E) F.add(seg_E(z));
      if (sites.atp(z).N) F.add(seg_N(z));
      if (sites.atp(z).NW) F.add(seg_NW(z));
    }
    F.show();
    F.pause();
  }
};

int main (int argc, char ** argv) {
    CLP  clp(argc, argv, "Facet percolation");
    auto p = clp.param("p", 0.5, "Percolation parameter");
    auto n = clp.param("n", size_t(50), "Domain size");
    clp.finalize();

  Perco P (n, p);
  P.show();
}