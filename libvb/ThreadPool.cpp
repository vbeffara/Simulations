#include <vb/Hub.h>
#include <vb/ThreadPool.h>
#include <atomic>
#include <boost/lockfree/queue.hpp>
#include <future>
#include <mutex>
#include <queue>
#include <thread>

namespace vb {
    void execute_seq(Project p) {
        for (auto pp : p.deps) execute_seq(pp);
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

        std::atomic<int>         n_total{int(std::max(p.deps.size(), 1ul))};
        std::mutex               m;
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
                            std::lock_guard<std::mutex> l(m);
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
} // namespace vb