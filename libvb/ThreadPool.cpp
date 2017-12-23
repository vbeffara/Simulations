#include <vb/ThreadPool.h>
#include <boost/lockfree/stack.hpp>
#include <future>
#include <thread>

namespace vb {
    void execute_seq(Project && p) {
        for (auto & pp : p.deps) execute_seq(std::move(pp));
        if (p.next) execute_seq((*p.next)());
    }

    void execute_asy(Project && p) {
        std::vector<std::future<void>> ts;
        for (auto & f : p.deps) ts.emplace_back(std::async([&f] { execute_asy(std::move(f)); }));
        for (auto & t : ts) t.get();
        if (p.next) execute_asy((*p.next)());
    }

    void execute_par(Project && p) {
        for (auto & pp : p.deps) pp.par = &p;

        boost::lockfree::stack<Project *> fringe;
        for (auto & pp : p.deps) fringe.push(&pp);
        if (p.deps.empty()) fringe.push(&p);

        std::vector<std::thread> runners;
        bool                     done = false;

        for (int i = 0; i < std::max(std::thread::hardware_concurrency(), 1u); ++i)
            runners.emplace_back([&] {
                Project * p = nullptr;
                while (true) {
                    if ((p == nullptr) && !fringe.pop(p)) {
                        if (done) break;
                        p = nullptr;
                        continue;
                    }
                    auto par = p->par;
                    if (auto n = p->next) {
                        *p     = (*n)();
                        p->par = par;
                    }
                    if (auto n = p->ndep; n > 0) {
                        for (int i = 0; i < n; ++i) {
                            p->deps[i].par = p;
                            if (i > 0) fringe.push(&(p->deps[i]));
                        }
                        p = &(p->deps[0]);
                        continue;
                    }
                    if (p->next) continue;
                    if (par == nullptr) {
                        done = true;
                        break;
                    }
                    p = (--(par->ndep) == 0) ? par : nullptr;
                }
            });
        for (auto & t : runners) t.join();
    }

    Project loop(int a, int b, const std::function<void(int)> & f, int l) {
        if (b - a <= l) {
            for (int i = a; i < b; ++i) f(i);
            return {};
        }
        int c = (a + b) / 2;
        return {[a, c, l, f] { return loop(a, c, f, l); }, [c, b, l, f] { return loop(c, b, f, l); }};
    }
} // namespace vb