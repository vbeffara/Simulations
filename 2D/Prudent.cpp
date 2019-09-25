#include <spdlog/spdlog.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace std;
using namespace vb;

auto main(int argc, char **argv) -> int {
    Hub H("Prudent random walk", argc, argv, "t=100000,n=10000");
    int T = H['t'], N = H['n'];

    int laststep = 0;

    double sd = 0.0, sd2 = 0.0;

    for (int u = 0; u < N; ++u) {
        int x = 0, y = 0, right = 0, top = 0, t = 0;
        while (t < T) {
            switch (prng.uniform_int(4)) {
            case 0:
                if ((x == right) || (laststep != 2)) {
                    ++x;
                    ++t;
                    laststep = 0;
                    if (x > right) right = x;
                }
                break;
            case 1:
                if ((y == top) || (laststep != 3)) {
                    ++y;
                    ++t;
                    laststep = 1;
                    if (y > top) top = y;
                }
                break;
            case 2:
                if ((y == top) && (laststep != 0)) {
                    --x;
                    ++t;
                    laststep = 2;
                }
                break;
            case 3:
                if ((x == right) && (laststep != 1)) {
                    ++x;
                    ++t;
                    laststep = 3;
                }
                break;
            }
        }

        double dev = (double(x + y) - double(t) * 3.0 / 7.0) / sqrt(double(t));
        spdlog::info("dev = {}", dev);
        sd += dev;
        sd2 += dev * dev;
    }

    H.output("Average", "a", sd / N);
    H.output("Variance", "v", sd2 / N);
}
