#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

auto main(int argc, char **argv) -> int {
    vb::Hub const H("Cookie random walk", argc, argv, "t=20,p=.67");
    size_t const  t = H['t'];
    double const  p = H['p'];

    std::vector<int> env;
    int64_t          X = 0;
    env.push_back(1);

    for (size_t i = 0; i < t; ++i) {
        std::cout << X << std::endl;
        if (env[size_t(X)] > 0) --env[size_t(X)];

        X += vb::prng.bernoulli(env[size_t(X)] > 0 ? p : .5) ? 1 : -1;

        if (X == -1) X = 1;
        if (X == int(env.size())) env.push_back(1);
    }
}
