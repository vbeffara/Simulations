#pragma once
#include <vb/ThreadPool.h>
#include <thread>

namespace vb {
    template <typename T> Project2 & Project2::add(T t) {
        ++ndep;
        deps.emplace_back();
        deps.back().next = std::move(t);
        deps.back().par  = this;
        return *this;
    }

    template <typename T> Project2 & Project2::then(T t) {
        next = std::move(t);
        return *this;
    }

    template <typename F> void Project2::run(F f) {
        auto                    p       = Project2{}.then(std::move(f));
        std::vector<Project2 *> fringe  = {&p};
        int                     n_total = 1;
        std::mutex              m;

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

                    auto np = (*(p->next))();

                    auto par = p->par;
                    np.par   = p->par;
                    *p       = np;

                    std::lock_guard<std::mutex> l(m);
                    if (p->deps.empty()) {
                        if (p->next) {
                            ++n_total;
                            fringe.push_back(p);
                        } else if (par != nullptr) {
                            --(par->ndep);
                            if (par->ndep == 0) {
                                ++n_total;
                                fringe.push_back(par);
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