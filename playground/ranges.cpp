#include <vb/Array.h>
#include <optional>

using namespace std;
using namespace vb;

template <typename T> struct iterate : public ranges::view_facade<iterate<T>> {
    iterate() = default;
    explicit iterate(T ii, std::function<T(T)> ff) : i(ii), f(ff) {}

    const T & read() const { return i; }
    bool      equal(ranges::default_sentinel /*unused*/) const { return false; }
    void      next() { i = f(std::move(i)); }

    T                   i;
    std::function<T(T)> f;
};

template <typename T> struct iterate_maybe : public ranges::view_facade<iterate_maybe<T>> {
    iterate_maybe() = default;
    explicit iterate_maybe(T ii, std::function<optional<T>(T)> ff) : i(ii), f(ff) {}

    const T & read() const { return *i; }
    bool      equal(ranges::default_sentinel /*unused*/) const { return !i; }
    void      next() { i = f(std::move(*i)); }

    std::optional<T>              i;
    std::function<optional<T>(T)> f;
};

int main(int /*argc*/, char ** /*argv*/) {
    auto is = iterate<int>(1, [](int i) { return i + 3; }) | rv::take(10);
    for (auto i : is) cout << i << '\n';

    auto js = iterate_maybe<int>(10, [](int i) { return (i > 0) ? optional<int>(i - 1) : optional<int>(); });
    for (auto j : js) cout << j << '\n';
}
