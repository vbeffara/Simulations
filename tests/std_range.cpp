#include <format>
#include <iostream>
#include <ranges>

// TODO: wait for <generator> header

namespace rv = std::ranges::views;

auto f(int i) -> double { return rand() % 1000 + i / 1000.0; }
auto good(double i) -> bool { return (int(i) % 100) != 0; }

auto main() -> int {
    for (const auto &x : rv::iota(0) | rv::transform(f) | rv::take_while(good)) { std::cout << x << '\n'; };
}
