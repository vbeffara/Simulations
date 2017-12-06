#include <functional>
#include <iostream>
#include <optional>
#include <vector>

using namespace std;

struct project {
    using job = std::function<project(void)>;
    project() = default;
    template <typename T, typename... Ts> project(T t, Ts... ts) : project(ts...) { add(t); }

    template <typename T> void add(T t) {
        project p;
        p.j   = make_unique<job>(std::move(t));
        p.par = this;
        deps.push_back(std::move(p));
        ++ndep;
    }

    template <typename T> void then(T t) { j = make_unique<job>(t); }

    std::vector<project> deps;
    std::unique_ptr<job> j;
    project *            par  = nullptr;
    int                  ndep = 0;
};

vector<project *> fringe;

project fakefib(int n, int d) {
    project p;
    if (n > 0) p.add(std::bind(fakefib, n - 1, d + 2));
    if (n > 1) p.add(std::bind(fakefib, n - 2, d + 2));
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

    project * par = p->par;
    *p            = (*(p->j))();
    p->par        = par;
    for (auto & pp : p->deps) pp.par = p;

    if (p->deps.size() > 0) {
        for (auto & t : p->deps) fringe.push_back(&t);
    } else if (p->j != nullptr) {
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
    project p;
    p.then([]() { return fakefib(5, 0); });
    fringe.push_back(&p);
    while (fringe.size() > 0) { step(); }
}