#include <vb/ThreadPool.h>
#include <atomic>
#include <future>
#include <mutex>
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

        std::vector<Project *> fringe;
        for (auto & pp : p.deps) fringe.push_back(&pp);
        if (p.deps.empty()) fringe.push_back(&p);

        int        n_total = fringe.size();
        std::mutex m;

        std::vector<std::thread> runners;
        for (int i = 0; i < std::max(std::thread::hardware_concurrency(), 1u); ++i)
            runners.emplace_back([&] {
                Project * p;
                while (n_total > 0) {
                    {
                        std::lock_guard<std::mutex> l(m);
                        if (fringe.empty()) continue;
                        p = fringe.back();
                        fringe.pop_back();
                    }

                    if (p->next) {
                        auto par = p->par;
                        *p       = (*(p->next))();
                        p->par   = par;
                    }

                    std::lock_guard<std::mutex> l(m);
                    if (p->ndep == 0) {
                        if (p->next) {
                            ++n_total;
                            fringe.push_back(p);
                        } else if (p->par != nullptr) {
                            --(p->par->ndep);
                            if (p->par->ndep == 0) {
                                ++n_total;
                                fringe.push_back(p->par);
                            }
                        }
                    } else {
                        for (auto & t : p->deps) {
                            t.par = p;
                            ++n_total;
                            fringe.push_back(&t);
                        }
                    }

                    --n_total;
                }
            });
        for (auto & t : runners) t.join();
    }
} // namespace vb