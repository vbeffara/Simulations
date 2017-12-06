#pragma once
#include <functional>
#include <vector>

namespace vb {
    struct Job;

    struct Project {
        Project(std::vector<Job> js, std::vector<Job> l) : jobs(std::move(js)), then(std::move(l)), njobs(jobs.size() + then.size()) {}
        Project() : Project({}, {}) {}

        std::vector<Job> jobs, then;
        int              njobs{0};
    };

    Project Parallel(std::vector<Job> js);
    Project Sequence(Job j1, Job j2);

    struct Job {
        template <typename T> Job(T t) : f(t) {}
        std::function<Project(void)> f;
        Project                      operator()() { return f(); }
    };

    void execute_plain(Job t);
    void execute_async(Job t);
    void execute_parallel(Job t);
} // namespace vb