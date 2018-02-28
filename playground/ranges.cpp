#include <vb/Array.h>
#include <optional>

using namespace std;
using namespace vb;

template <typename T, typename F> auto iterate(T t, F f) {
    return rv::generate([t, f, ap = false]() mutable -> T const & {
        if (std::exchange(ap, true)) t = f(std::move(t));
        return t;
    });
}

template <typename T, typename F> auto iterate_m(T t, F f) {
    return iterate(std::optional<T>(t), [f](auto t) { return f(std::move(*t)); }) |
           rv::take_while([](auto x) { return x != std::nullopt; }) | rv::indirect;
}

int main(int /*argc*/, char ** /*argv*/) {
    auto is = iterate(1, [](int i) { return i + 3; }) | rv::take(10);
    for (auto i : is) cout << i << '\n';

    auto ls = iterate_m(10, [](int i) { return (i > 0) ? optional<int>(i - 1) : nullopt; });
    for (auto l : ls) cout << l << '\n';
}
