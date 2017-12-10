#pragma once
#include <functional>
#include <optional>
#include <vector>

namespace vb {
    struct Project2 {
        template <typename... Ts> Project2(Ts... ts) { (add(ts), ...); } // NOLINT

        template <typename T> Project2 & add(T t) {
            ++ndep;
            deps.emplace_back();
            deps.back().next = std::move(t);
            deps.back().par  = this;
            return *this;
        }

        template <typename T> Project2 & then(T t) {
            next = std::move(t);
            return *this;
        }

        std::vector<Project2>                    deps;
        std::optional<std::function<Project2()>> next;
        Project2 *                               par  = nullptr;
        int                                      ndep = 0;
    };

    void execute_seq(Project2 p);
    void execute_par(Project2 p);
    void execute_asy(Project2 p);
} // namespace vb