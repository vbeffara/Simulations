#pragma once
#include <functional>
#include <optional>
#include <vector>

namespace vb {
    struct Job;

    struct Project {
        Project(std::vector<Job> js) : jobs(std::move(js)), njobs(jobs.size()) {}
        Project() = default;

        std::vector<Job> jobs;
        int              njobs{0};
    };

    Project Parallel(std::vector<Job> js);

    struct Job {
        template <typename T> Job(T t) : f(t) {}
        std::function<Project(void)> f;
        Project                      operator()() { return f(); }
    };

    struct Project2 {
        template <typename... Ts> Project2(Ts... ts) { (add(ts), ...); } // NOLINT

        template <typename F> Project2 & add(F f);
        template <typename F> Project2 & then(F f);

        template <typename F> static void run(F f);

        std::vector<Project2>                    deps;
        std::optional<std::function<Project2()>> next;
        Project2 *                               par  = nullptr;
        int                                      ndep = 0;
    };

    void execute_plain(Job t);
    void execute_async(Job t);
    void execute_parallel(Job t);
} // namespace vb

#include <vb/impl/ThreadPool.hxx>