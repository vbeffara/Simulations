#include <iostream>
#include <range/v3/all.hpp>
#include <range/v3/experimental/utility/generator.hpp>

auto good(double i) -> bool { return (int(i) % 100) != 0; }

auto rands = ranges::views::generate([i = 0]() mutable { return rand() % 1000 + (++i) / 1000.0; });

auto rands2() -> ranges::experimental::generator<double> {
    int i = 0;
    while (true) {
        ++i;
        co_yield rand() % 1000 + i / 1000.0;
    }
}

auto main() -> int {
    for (const auto i : rands | ranges::views::take_while(good)) { std::cout << i << '\n'; }
    std::cout << "===========\n";
    for (const auto i : rands2() | ranges::views::take_while(good)) { std::cout << i << '\n'; }
}
