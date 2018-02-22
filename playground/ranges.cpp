#include <vb/Array.h>

using namespace ranges;
using namespace std;
using namespace vb;

int main(int /*argc*/, char ** /*argv*/) {
    Array<int> A(4, 3);
    cout << "Ref:    ";
    for (auto c : A.coos()) cout << c << ' ';
    cout << '\n';

    cout << "Ranges: ";
    auto zs = view::cartesian_product(view::ints(0LL, A.ww), view::ints(0LL, A.hh)) |
              view::transform([](auto xy) { return std::make_from_tuple<coo>(xy); });
    for (auto z : zs) cout << z << ' ';
    cout << '\n';
}
