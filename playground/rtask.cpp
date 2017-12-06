#include <functional>
#include <iostream>
#include <optional>
#include <vector>

using namespace std;

struct project {
    project() = default;
    template <typename... Ts> explicit project(Ts... ts) { (add(ts), ...); }

    template <typename T> project & add(T t) {
        ++ndep;
        deps.emplace_back();
        deps.back().j   = std::move(t);
        deps.back().par = this;
        return *this;
    }

    template <typename T> project & then(T t) {
        j = std::move(t);
        return *this;
    }

    vector<project>                   deps;
    optional<function<project(void)>> j;
    project *                         par  = nullptr;
    int                               ndep = 0;
};

vector<project *> fringe;

project fakefib(int n, int d) {
    project p;
    if (n > 0) p.add([=] { return fakefib(n - 1, d + 2); });
    if (n > 1) p.add([=] { return fakefib(n - 2, d + 2); });
    p.then([n, d]() -> project {
        for (int i = 0; i < d; ++i) std::cout << ' ';
        std::cout << n << endl;
        return {};
    });
    return p;
}

void step() {
    project * p = fringe.back();
    fringe.pop_back();

    auto par = p->par;
    *p       = (*(p->j))();
    p->par   = par;

    for (auto & t : p->deps) {
        t.par = p;
        fringe.push_back(&t);
    }
    if (!(p->deps.empty())) return;
    if (p->j) {
        fringe.push_back(p);
    } else if (par != nullptr) {
        --(par->ndep);
        if (par->ndep == 0) {
            par->deps.clear();
            fringe.push_back(par);
        }
    }
}

int main() {
    auto p = project{}.then([]() { return fakefib(5, 0); });
    fringe.push_back(&p);
    while (!fringe.empty()) step();
}