#include <vb/CoarseImage.h>
#include <vb/data/Queue.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;

auto cost() -> double { return prng.exponential(); }

class FPP : public CoarseImage {
  public:
  FPP(const std::string &title, size_t n, bool trace_, bool invasion_, bool twostep_)
      : CoarseImage(title, {n, n}, size_t(pow(double(n), .33))), trace(trace_), invasion(invasion_), twostep(twostep_) {
    pq.push({{n / 2, n / 2}, cost()});
    if (twostep) pq.push({{n / 2 + 1, n / 2}, cost()});
  };

  void spread(double t, const ucoo &z) {
    for (unsigned d = 0; d < 4; ++d) {
      auto zz = z + dz[d];
      if (!at(zz)) pq.push({zz, t + cost()});
    }
  }

  void run() {
    while (true) {
      if (trace) std::cout << area << " " << pq.q.size() << std::endl;

      auto        pt = pq.get();
      const ucoo &z  = pt.v;
      if (!at(z)) {
        put(z, true);
        ++area;

        double const curtime = invasion ? 0.0 : pt.t;

        int deg = 1;
        if (twostep)
          for (unsigned d = 0; d < 4; ++d) deg += (at(z + dz[d]) ? 1 : 0);

        for (int ii = 0; ii < deg; ++ii) spread(curtime, z);
        if (twostep)
          for (unsigned d = 0; d < 4; ++d) {
            auto zz = z + dz[d];
            if (at(zz)) spread(curtime, zz);
          }

        if ((z.x == 1) || (z.y == 1) || (z.x == size.x - 2) || (z.y == size.y - 2)) break;
      }
    }
  }

  int         area{0};
  bool        trace, invasion, twostep;
  Queue<ucoo> pq;
};

auto main(int argc, char **argv) -> int {
  CLP  clp(argc, argv, "First-passage percolation");
  auto n = clp.param("n", 5000UL, "grid size");
  auto i = clp.flag("i", "invasion percolation mode");
  auto twostep = clp.flag("2", "two-step mode");
  auto t = clp.flag("t", "trace output");
  clp.finalize();

  FPP F(clp.title, n, t, i, twostep);
  F.show();
  F.run();
  F.output(clp.title);
}
