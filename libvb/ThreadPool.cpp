#include <vb/Hub.h>
#include <vb/ThreadPool.h>
#include <atomic>
#include <future>
#include <iostream>
#include <mutex>
#include <thread>

namespace vb {
    Project Parallel(std::vector<Job> js) { return {std::move(js)}; }

    void execute_plain(Job t) {
        for (auto f : t().jobs) execute_plain(f);
    }

    void execute_async(Job t) {
        std::vector<std::future<void>> ts;
        for (auto f : t().jobs) ts.emplace_back(std::async([f]() { execute_async(f); }));
    }

    void execute_parallel(Job t) {
        Project    P{{std::move(t)}};
        std::mutex m;

        std::vector<std::thread> runners;
        for (int i = 0; i < std::max(std::thread::hardware_concurrency(), 1u); ++i)
            runners.emplace_back([&] {
                std::optional<Job> t;
                while (P.njobs > 0) {
                    if (!t) {
                        std::lock_guard<std::mutex> l(m);
                        if (P.jobs.empty()) continue;
                        t = std::move(P.jobs.back());
                        P.jobs.pop_back();
                    }
                    auto ts = (*t)();
                    {
                        std::lock_guard<std::mutex> l(m);
                        P.njobs += ts.jobs.size() - 1;
                    }
                    if (ts.jobs.empty()) {
                        t = {};
                    } else {
                        for (int i = 0; i < ts.jobs.size() - 1; ++i) {
                            std::lock_guard<std::mutex> l(m);
                            P.jobs.push_back(std::move(ts.jobs[i]));
                        }
                        t = std::move(ts.jobs.back());
                    }
                }
            });
        for (auto & t : runners) t.join();
    }

    void execute_seq(Project2 p) {
        for (auto pp : p.deps) execute_seq(pp);
        if (p.next) execute_seq((*p.next)());
    }

    void execute_par(Project2 p) {
        for (auto & pp : p.deps) pp.par = &p;

        std::vector<Project2 *> fringe;
        for (auto & pp : p.deps) fringe.push_back(&pp);
        if (p.deps.empty()) fringe.push_back(&p);

        int        n_total = fringe.size();
        std::mutex m;

        std::vector<std::thread> runners;
        for (int i = 0; i < std::max(std::thread::hardware_concurrency(), 1u); ++i)
            runners.emplace_back([&] {
                Project2 * p;
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