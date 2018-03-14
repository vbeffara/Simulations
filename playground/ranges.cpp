#include <iostream>
#include <range/v3/all.hpp>
#include <vector>

const std::vector<int> p4{0, 1, 2, 3};

auto perms() {
    return ranges::view::generate([t = p4]() -> const std::vector<int> & {
        std::cout << "Returning vector of size " << t.size() << '\n';
        return t;
    });
}

auto perms_n(int n) {
    return ranges::view::generate_n(
        [t = p4]() -> const std::vector<int> & {
            std::cout << "Returning vector of size " << t.size() << '\n';
            return t;
        },
        n);
}

int main(int /*argc*/, char ** /*argv*/) {
    auto ps = perms();
    for (const auto & p : ps | ranges::view::take(2)) std::cout << " -> Obtained vector of size " << p.size() << '\n';
    std::cout << "==========\n";
    for (const auto & p : perms() | ranges::view::take(2)) std::cout << " -> Obtained vector of size " << p.size() << '\n';
    std::cout << "==========\n";
    for (const auto & p : perms_n(2)) std::cout << " -> Obtained vector of size " << p.size() << '\n';
}

// template <typename T, typename F> auto iterate(T t, F f) {
//     return rv::generate([t = std::move(t), f, ap = false]() mutable -> T {
//         if (std::exchange(ap, true)) t = f(std::move(t));
//         return t;
//     });
// }

// template <typename T, typename F> auto iterate_m(T t, F f) {
//     return iterate(std::optional<T>(t), [f](auto i) { return f(*i); }) | rv::take_while([](auto x) { return x != std::nullopt; }) |
//            rv::transform([](auto x) { return *x; });
// }

// auto perms(int n) {
//     return iterate_m(Permutation(n), [](auto p) { return next_permutation(p.begin(), p.end()) ? optional<Permutation>(p) : nullopt; });
// }
