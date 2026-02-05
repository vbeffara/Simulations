#include <vb/util/CLP.h>
#include <vb/util/PRNG.h>

auto main(int argc, char **argv) -> int {
    vb::CLP clp(argc, argv, "Cookie random walk");
    auto    t = clp.param("t", size_t(20), "Number of steps");
    auto    p = clp.param("p", 0.67, "Cookie probability");
    clp.finalize();

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
