#pragma once
#include <vb/Hub.h>
#include <atomic>
#include <boost/lockfree/stack.hpp>
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

        using ftp = std::function<Project()>;

        Project() = default;
        Project(ftp && t) : next(std::move(t)) {}
        Project(ftp && t1, ftp && t2) : ndep(2) {
            deps[0]      = std::make_unique<Project>(std::move(t1));
            deps[1]      = std::make_unique<Project>(std::move(t2));
            deps[0]->par = this;
            deps[1]->par = this;
        }

        template <typename T> Project(T && t) : Project(ftp(std::move(t))) {}

        Project & then(ftp && t) {
            next = std::move(t);
            return *this;
        }

        std::unique_ptr<Project> deps[2];
        std::optional<ftp>       next;
        Project *                par = nullptr;
        counter                  ndep;
    }; // namespace vb

    void project_runner(boost::lockfree::stack<Project *> & fringe, bool & done);

    void execute_seq(Project && p);
    void execute_asy(Project && p);
    void execute_run(Project && p);
    void execute_par(Project && p);

    Project loop(int a, int b, const std::function<void(int)> & f, int l = 100);
} // namespace vb