#pragma once
#include <atomic>
#include <functional>
#include <optional>
#include <vector>

namespace vb {
    struct Project {
        struct counter : public std::atomic<int> {
            counter(int o = 0) : std::atomic<int>(o) {}
            counter(const counter & o) : std::atomic<int>(o.load()) {}
            counter & operator=(const counter & o) { return store(o.load()), *this; }
        };

        template <typename... Ts> Project(Ts... ts) { (add(ts), ...); } // NOLINT

        template <typename T> Project & add(T t) {
            ++ndep;
            deps.emplace_back();
            deps.back().next = std::move(t);
            deps.back().par  = this;
            return *this;
        }

        template <typename T> Project & then(T t) {
            next = std::move(t);
            return *this;
        }

        std::vector<Project>                    deps;
        std::optional<std::function<Project()>> next;
        Project *                               par = nullptr;
        counter                                 ndep;
    }; // namespace vb

    void execute_seq(Project p);
    void execute_par(Project p);
    void execute_asy(Project p);

    Project loop(int a, int b, const std::function<void(int)> & f, int l = 100);
} // namespace vb