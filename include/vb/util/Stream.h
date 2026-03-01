#pragma once
#include <coroutine>
#include <exception>
#include <vector>

namespace vb {
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

  inline auto partitions(size_t n, size_t m = 1) -> Stream2<std::vector<size_t>> {
    for (size_t i = m; i < n; ++i) {
      for (const auto &p : partitions(n - i, i)) {
        std::vector<size_t> out({i});
        for (auto j : p) out.push_back(j);
        co_yield (out);
      }
    }
    if (n >= m) co_yield (std::vector<size_t>{n});
  }

  inline auto tuples(size_t k, size_t n) -> Stream2<std::vector<size_t>> {
    if (k == 0) {
      co_yield (std::vector<size_t>{});
    } else {
      for (size_t i = 0; i < n; ++i) {
        for (const auto &c : tuples(k - 1, n - 1)) {
          std::vector<size_t> out({i});
          for (auto j : c) out.push_back(j < i ? j : j + 1);
          co_yield (out);
        }
      }
    }
  }

  inline auto cycles(size_t k, size_t n) -> Stream2<std::vector<size_t>> {
    for (size_t i = 0; i < n - k + 1; ++i) {
      for (const auto &c : tuples(k - 1, n - i - 1)) {
        std::vector<size_t> out(1, i);
        for (auto j : c) out.push_back(i + j + 1);
        co_yield (out);
      }
    }
  }
} // namespace vb
