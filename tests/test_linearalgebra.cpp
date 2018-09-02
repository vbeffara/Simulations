#include <vb/LinearAlgebra.h>
#include <vb/mp.h>
#include <iomanip>

using namespace vb;
using namespace std;

template <typename T> void test(string s) {
    Matrix<T> m(3, 3);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j) m(i, j) = int(pow(2 * i + 1, j));
    Vector<T> v(3);
    for (int i = 0; i < 3; ++i) v(i) = int(3 * i - 2);
    auto x = solve(m, v);

    // H.L->info ("{} | Matrix   m = {}", s, m);
    // H.L->info ("{} | Vector   v = {}", s, v);
    H.L->info("{} | Solution x = {}", s, x.transpose());
}

int main() {
    test<double>("double   ");
    test<cpx>("cpx      ");
    test<real_t>("real_t   ");
    test<complex_t>("complex_t");
}
