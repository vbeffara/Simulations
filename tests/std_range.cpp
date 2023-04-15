#include <format>
#include <iostream>
#include <ranges>

// TODO: wait for <generator> header

using namespace std::ranges;

auto f(int i) -> double { return rand() % 1000 + i / 1000.0; }
auto good(double i) -> bool { return (int(i) % 100) != 0; }

auto main() -> int {
    auto goods = views::iota(0) | views::transform(f) | views::take_while(good);
    for_each(goods, [](double x) { std::cout << x << '\n'; });
}
