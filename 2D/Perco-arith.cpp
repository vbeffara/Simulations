#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

class Perco : public Image {
  public:
  Perco(const Hub &H, size_t n, double a) : Image(H.title, {n, n}) {
    // Initialize as all white
    for (size_t x = 0; x < n; ++x)
      for (size_t y = 0; y < n; ++y) put({x, y}, WHITE);

    // Sieve to get primes
    vector<bool> isprime(n * n, true);
    isprime[0] = false;
    isprime[1] = false;
    for (int i = 2; i < n * n; ++i) {
      if (!(isprime[i])) continue;
      for (int j = 2; i * j < n * n; ++j) isprime[i * j] = false;
    }

    // Simulation
    for (int p = 2; p < n * n; ++p) {
      if (!isprime[p]) continue;
      size_t rp = a * log(p);
      int x = prng.uniform_int(p), y = prng.uniform_int(p);
      for (size_t i = x; i < n; i += p)
        for (size_t j = y; j < n; j += p)
          for (size_t ii = i; ii <= i + rp; ++ii)
            for (size_t jj = j; jj <= j + rp; ++jj)
                if ((ii<n) && (jj<n)) put({ii, jj}, BLACK);
    }

    // Fill the cluster of the midpoint
    size_t m = n/2;
    while ((m<n) && (at({m, n/2}) == BLACK)) ++m;
    if (m<n) fill({m, n / 2}, RED);
    show();
  }
};

auto main(int argc, char **argv) -> int {
  Hub const H("Percolation", argc, argv, "n=500,a=.5");
  Perco     P(H, H['n'], H['a']);
  P.pause();
}
