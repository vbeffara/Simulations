#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

int main(int argc, char **argv) {
    vb::Hub H("Cookie random walk", argc, argv, "t=20,p=.67");
    int     t = H['t'];
    double  p = H['p'];

    std::vector<int> env;
    int              X = 0;
    env.push_back(1);

    for (int i = 0; i < t; ++i) {
        std::cout << X << std::endl;
        if (env[X] > 0) --env[X];

        X += vb::prng.bernoulli(env[X] > 0 ? p : .5) ? 1 : -1;

        if (X == -1) X = 1;
        if (X == int(env.size())) env.push_back(1);
    }
}
