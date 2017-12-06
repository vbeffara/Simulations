#include <functional>
#include <iostream>
#include <vector>

using namespace std;

struct job;

struct project {
    std::vector<project> deps;
    std::unique_ptr<job> j;
};

struct job : public function<project()> {
    template <typename T> job(T t) : function<project()>(t) {}
};

project single(job j) { return project{{}, make_unique<job>(std::move(j))}; }

project fakefib(int n, int d) {
    project p;
    if (n < 0) return p;
    p.deps.push_back(single([n, d]() { return fakefib(n - 2, d + 2); }));
    p.deps.push_back(single([n, d]() { return fakefib(n - 1, d + 2); }));
    p.j = make_unique<job>([n, d]() {
        for (int i = 0; i < d; ++i) std::cout << ' ';
        std::cout << n << endl;
        return project{};
    });
    return p;
}

void step(project & p) {
    if (p.deps.size()) {
        step(p.deps.back());
        auto & pp = p.deps.back();
        if ((pp.deps.size() == 0) && (pp.j == nullptr)) p.deps.pop_back();
    } else if (p.j) {
        p = (*(p.j))();
    }
}

int main() {
    project p = fakefib(4, 0);
    while ((p.deps.size() > 0) || (p.j != nullptr)) step(p);
}