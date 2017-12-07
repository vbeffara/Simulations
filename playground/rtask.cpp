#include <functional>
#include <iostream>
#include <optional>
#include <vector>

struct project {
    template <typename... Ts> project(Ts... ts) { (add(ts), ...); } // NOLINT

    template <typename T> project & add(T t) {
        ++ndep;
        deps.emplace_back();
        deps.back().next = std::move(t);
        deps.back().par  = this;
        return *this;
    }

    template <typename T> project & then(T t) {
        next = std::move(t);
        return *this;
    }

    std::vector<project>                    deps;
    std::optional<std::function<project()>> next;
    project *                               par  = nullptr;
    int                                     ndep = 0;
};

template <typename F> void run(F f) {
    auto                   p      = project{}.then(std::move(f));
    std::vector<project *> fringe = {&p};

    while (!fringe.empty()) {
        project * p = fringe.back();
        fringe.pop_back();

        auto par = p->par;
        *p       = (*(p->next))();
        p->par   = par;

        for (auto & t : p->deps) {
            t.par = p;
            fringe.push_back(&t);
        }
        if (!(p->deps.empty())) continue;
        if (p->next)
            fringe.push_back(p);
        else if (par != nullptr) {
            --(par->ndep);
            if (par->ndep == 0) {
                par->deps.clear();
                fringe.push_back(par);
            }
        }
    }
}

project fakefib(int n, int d) {
    project p;
    if (n > 0) p.add([=] { return fakefib(n - 1, d + 2); });
    if (n > 1) p.add([=] { return fakefib(n - 2, d + 2); });
    p.then([n, d]() -> project {
        for (int i = 0; i < d; ++i) std::cout << ' ';
        std::cout << n << '\n';
        return {};
    });
    return p;
}

int main() {
    run([] { return fakefib(5, 0); });
}