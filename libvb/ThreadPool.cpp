#include <vb/ThreadPool.h>
#include <future>
#include <thread>

namespace vb {
    void run_par(const std::function<void(BLST &, std::shared_ptr<Will>)> &f) {
        BLST                     S;
        bool                     done = false;
        std::vector<std::thread> ts(std::max(std::thread::hardware_concurrency(), 1u));
        for (auto &t : ts)
            t = std::thread([&] {
                while (!done)
                    if (Work op; S.pop(op)) op();
            });
        f(S, std::make_shared<Will>([&done] { done = true; }));
        for (auto &t : ts) t.join();
    }

    void loop_go(BLST &S, std::shared_ptr<Will> parent, int a, int b, std::function<void(int)> f, int l) {
        if (b - a <= l) {
            for (int i = a; i < b; ++i) f(i);
            return;
        }
        int c = (a + b) / 2;
        S.push([=, &S] { loop_go(S, parent, a, c, f, l); });
        S.push([=, &S] { loop_go(S, parent, c, b, f, l); });
    };

    void loop_par(int a, int b, std::function<void(int)> f, int l) {
        run_par([=](auto &S, auto p) { loop_go(S, p, a, b, f, l); });
    }

    Project::Project(ftp &&t) : next(std::move(t)) {}

    Project::Project(ftp &&t1, ftp &&t2) : deps(2), ndep(2) {
        deps[0]     = std::move(t1);
        deps[1]     = std::move(t2);
        deps[0].par = this;
        deps[1].par = this;
    }

    Project::Project(ftp &&t1, ftp &&t2, ftp &&n) : Project(std::move(t1), std::move(t2)) { next = std::move(n); }

    void project_runner(boost::lockfree::stack<Project *> &fringe, bool &done) {
        Project *p = nullptr;
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
    }

    void execute_seq(Project &&p) {
        for (int i = 0; i < p.ndep; ++i) execute_seq(std::move(p.deps[i]));
        if (p.next) execute_seq((*p.next)());
    }

    void execute_asy(Project &&p) {
        std::vector<std::future<void>> ts;
        ts.reserve(p.ndep);
        for (int i = 0; i < p.ndep; ++i) ts.emplace_back(std::async([&p, i] { execute_asy(std::move(p.deps[i])); }));
        for (auto &t : ts) t.get();
        if (p.next) execute_asy((*p.next)());
    }

    void execute_run(Project &&p) {
        for (int i = 0; i < p.ndep; ++i) p.deps[i].par = &p;
        boost::lockfree::stack<Project *> fringe;
        for (int i = 0; i < p.ndep; ++i) fringe.push(&(p.deps[i]));
        if (p.ndep == 0) fringe.push(&p);
        bool done = false;
        project_runner(fringe, done);
    }

    void execute_par(Project &&p) {
        for (int i = 0; i < p.ndep; ++i) p.deps[i].par = &p;
        boost::lockfree::stack<Project *> fringe;
        for (int i = 0; i < p.ndep; ++i) fringe.push(&(p.deps[i]));
        if (p.ndep == 0) fringe.push(&p);

        std::vector<std::thread> runners;
        bool                     done = false;

        for (int i = 0; i < std::max(std::thread::hardware_concurrency(), 1u); ++i)
            runners.emplace_back([&fringe, &done] { project_runner(fringe, done); });
        for (auto &t : runners) t.join();
    }

    Project loop(int a, int b, const std::function<void(int)> &f, int l) {
        if (b - a <= l) {
            for (int i = a; i < b; ++i) f(i);
            return {};
        }
        int c = (a + b) / 2;
        return {[a, c, l, f] { return loop(a, c, f, l); }, [c, b, l, f] { return loop(c, b, f, l); }};
    }
} // namespace vb
