#pragma once
#include <functional>
#include <optional>
#include <vector>

namespace vb {
    struct Project {
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
        Project *                               par  = nullptr;
        int                                     ndep = 0;
    };

    void execute_seq(Project p);
    void execute_par(Project p);
    void execute_asy(Project p);
} // namespace vb