#pragma once
#include <range/v3/all.hpp>
#include <range/v3/experimental/utility/generator.hpp>

namespace vb {
    template <typename T> using Stream = ranges::experimental::generator<T>;

    template <typename T> struct Stream2 {
        struct promise_type {
            T    value;
            auto get_return_object() { return Stream2{this}; }
            auto initial_suspend() { return std::suspend_always{}; }
            auto final_suspend() noexcept { return std::suspend_always{}; }
            void unhandled_exception() { std::terminate(); }
            void return_void() {}
            auto yield_value(T value) {
                this->value = value;
                return std::suspend_always{};
            }
        };

        using handle_type = std::coroutine_handle<promise_type>;
        handle_type coro;

        Stream2(promise_type *p) : coro(handle_type::from_promise(*p)) {}

        T operator()() { return coro.promise().value; }

        Stream2 begin() {
            coro.resume();
            return *this;
        }
        Stream2 end() { return Stream2{nullptr}; }

        bool operator==(const Stream2 &other) const { return !coro || coro.done(); }

        bool operator!=(const Stream2 &other) const { return !(*this == other); }

        Stream2 &operator++() {
            if (coro) coro.resume();
            return *this;
        }

        T operator*() const { return coro.promise().value; }
    };

    auto partitions(size_t n, size_t m = 1) -> Stream<std::vector<size_t>>; // Partitions of n with piece size at least m
    auto tuples(size_t k, size_t n) -> Stream<std::vector<size_t>>;         // Ordered, distinct k-tuples in [0,n-1]
    auto cycles(size_t k, size_t n) -> Stream<std::vector<size_t>>;         // k-tuples up to cyclic permutation
} // namespace vb
