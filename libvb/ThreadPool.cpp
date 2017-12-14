#include <vb/ThreadPool.h>
#include <boost/lockfree/queue.hpp>
#include <future>
#include <thread>

namespace vb {
    void execute_seq(Project p) {
        for (const auto & pp : p.deps) execute_seq(pp);
        if (p.next) execute_seq((*p.next)());
    }

    void execute_asy(Project p) {
        std::vector<std::future<void>> ts;
        for (auto & f : p.deps) ts.emplace_back(std::async([&f] { execute_asy(f); }));
        for (auto & t : ts) t.get();
        if (p.next) execute_asy((*p.next)());
    }

    void execute_par(Project p) {
        for (auto & pp : p.deps) pp.par = &p;

        boost::lockfree::queue<Project *> fringe;
        for (auto & pp : p.deps) fringe.push(&pp);
        if (p.deps.empty()) fringe.push(&p);

        Project::counter         n_total{int(std::max(p.deps.size(), 1ul))};
        std::vector<std::thread> runners;

        for (int i = 0; i < std::max(std::thread::hardware_concurrency(), 1u); ++i)
            runners.emplace_back([&] {
                Project * p;
                while (n_total > 0) {
                    if (!fringe.pop(p)) continue;

                    if (p->next) {
                        auto par = p->par;
                        *p       = (*(p->next))();
                        p->par   = par;
                    }

                    if (p->ndep == 0) {
                        if (p->next) {
                            ++n_total;
                            fringe.push(p);
                        } else if (p->par != nullptr) {
                            if (--(p->par->ndep) == 0) {
                                ++n_total;
                                fringe.push(p->par);
                            }
                        }
                    } else {
                        for (auto & t : p->deps) {
                            t.par = p;
                            ++n_total;
                            fringe.push(&t);
                        }
                    }

                    --n_total;
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
        return {[a, c, l, &f] { return loop(a, c, f, l); }, [c, b, l, &f] { return loop(c, b, f, l); }};
    }
} // namespace vb