#include <spdlog/spdlog.h>
#include <vb/math/LinearAlgebra.h>
#include <vb/util/Hub.h>
#include <vb/util/mp.h>

using namespace vb;
using namespace std;

template <typename T> void test(string s) {
    Matrix<T> m(3, 3);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) m(i, j) = int(pow(2 * i + 1, j));
    Vector<T> v(3);
    for (int i = 0; i < 3; ++i) v(i) = int(3 * i - 2);
    auto x = solve(m, v);

    // spdlog::info ("{} | Matrix   m = {}", s, m);
    // spdlog::info ("{} | Vector   v = {}", s, v);
    spdlog::info("{} | Solution x = {}", s, x.transpose());
}

auto main(int argc, char **argv) -> int {
    Hub H("Testing linear algebra", argc, argv);
    test<double>("double   ");
    test<cpx>("cpx      ");
    test<real_t>("real_t   ");
    test<complex_t>("complex_t");
}
