#include <vb/Bitmap.h>
#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class Perco : public Image {
  public:
  Perco(const std::string &t, size_t n, double a, size_t p0, bool all) : Image(t, {n, n}) {
    // Initialize as all white
    for (size_t x = 0; x < n; ++x)
      for (size_t y = 0; y < n; ++y) put({x, y}, WHITE);

    // Sieve to get primes
    vector<bool> isprime(n * n, true);
    isprime[0] = false;
    isprime[1] = false;
    for (int i = 2; i < isprime.size(); ++i) {
      if (!(isprime[i])) continue;
      for (int j = 2; i * j < isprime.size(); ++j) isprime[i * j] = false;
    }

    // Simulation
    for (int p = p0; p < isprime.size(); ++p) {
      if (!(all || isprime[p])) continue;
      size_t rp = a * log(p);
      int    x = prng.uniform_int(p), y = prng.uniform_int(p);
      for (size_t i = x; i < n; i += p)
        for (size_t j = y; j < n; j += p)
          for (size_t ii = i; ii <= i + rp; ++ii)
            for (size_t jj = j; jj <= j + rp; ++jj)
              if ((ii < n) && (jj < n)) put({ii, jj}, BLACK);
    }

    // Fill the clusters from the left edge
    for (size_t y = 0; y < n; ++y)
      if (at({0, y}) == WHITE) fill({0, y}, RED);
  }
};

auto main(int argc, char **argv) -> int {
  CLP  clp(argc, argv, "Arithmetic percolation");
  auto n = clp.param("n", size_t(500), "Simulation size");
  auto a = clp.param("a", double(0.0), "Size of the holes (as a factor of log(p))");
  auto p = clp.param("p", size_t(2), "Skipped scales");
  auto x = clp.flag("x", "Use all integers instead of primes");
  clp.finalize();
  Perco P(clp.title, n, a, p, x);
  P.show();
  P.pause();
}
