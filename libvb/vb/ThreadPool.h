#pragma once

#include <functional>
#include <vector>

namespace vb {
    struct Job;

    using Project = std::vector<Job>;

    struct Job : public std::function<Project(void)> {
        template <typename T> Job(T t) : std::function<Project(void)>(t) {}
        using std::function<Project(void)>::operator();
    };

    void execute_plain(Job t);

    void execute_parallel(Job t);
} // namespace vb