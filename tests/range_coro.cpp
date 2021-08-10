#include <iostream>
#include <range/v3/all.hpp>
#include <range/v3/experimental/utility/generator.hpp>

bool good (double i) { return int(i)%100; }

auto rands = ranges::views::generate([i = 0]() mutable { return rand() % 1000 + (++i) / 1000.0; });

ranges::experimental::generator<double> rands2() {
    int i = 0;
    while (true) co_yield rand() % 1000 + (++i) / 1000.0;
}

int main() {
    for (const auto i : rands | ranges::views::take_while(good)) { std::cout << i << '\n'; }
    std::cout << "===========\n";
    for (const auto i : rands2() | ranges::views::take_while(good)) { std::cout << i << '\n'; }
}
