#pragma once
#include <vb/Hub.h>
#include <atomic>
#include <boost/lockfree/stack.hpp>
#include <functional>
#include <optional>
#include <vector>

namespace vb {
    using Work = std::function<void()>;
    using BLST = boost::lockfree::stack<Work>;

    struct Will : public Work {
        using Work::Work;
        ~Will() { (*this)(); }
    };

    void run_par(const std::function<void(BLST &, std::shared_ptr<Will>)> &f);

    void loop_par(int a, int b, const std::function<void(int)> &f, int l = 100);

    struct Project {
        struct counter : public std::atomic<int> {
            counter(int o = 0) : std::atomic<int>(o) {}
            counter(const counter &o) : std::atomic<int>(o.load()) {}
            counter &operator=(const counter &o) { return store(o.load()), *this; }
        };

        using ftp = std::function<Project()>;

        Project()           = default;
        Project(Project &&) = default;
        Project(ftp &&t);
        Project(ftp &&t1, ftp &&t2);
        Project(ftp &&t1, ftp &&t2, ftp &&n);

        template <typename T> Project(T &&t) : Project(ftp(std::move(t))) {}

        Project &operator=(Project &&o) = default;

        std::vector<Project> deps;
        std::optional<ftp>   next;
        Project *            par = nullptr;
        counter              ndep;
    }; // namespace vb

    void project_runner(boost::lockfree::stack<Project *> &fringe, bool &done);

    void execute_seq(Project &&p);
    void execute_asy(Project &&p);
    void execute_run(Project &&p);
    void execute_par(Project &&p);

    Project loop(int a, int b, const std::function<void(int)> &f, int l = 100);
} // namespace vb
