#include <functional>
#include <iostream>
#include <vector>

using namespace std;

struct job;

struct project {
    std::vector<project> deps;
    std::unique_ptr<job> j;
    project *            par  = nullptr;
    int                  ndep = 0;
};

vector<project *> fringe;

struct job : public function<project()> {
    template <typename T> job(T t) : function<project()>(t) {}
};

project single(job j) { return project{{}, make_unique<job>(std::move(j)), nullptr, 0}; }

project fakefib(int n, int d) {
    project p;
    if (n < 0) return p;
    p.deps.push_back(single([n, d]() { return fakefib(n - 2, d + 2); }));
    p.deps.push_back(single([n, d]() { return fakefib(n - 1, d + 2); }));
    p.ndep = 2;
    p.j    = make_unique<job>([n, d]() {
        for (int i = 0; i < d; ++i) std::cout << ' ';
        std::cout << n << endl;
        return project{};
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
    project p = single([]() { return fakefib(4, 0); });
    fringe.push_back(&p);
    while (fringe.size() > 0) { step(); }
}