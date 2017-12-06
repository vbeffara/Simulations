#include <functional>
#include <iostream>
#include <vector>

using namespace std;

struct job;

struct project {
    std::vector<project *> deps;
    job *                  j = nullptr;
};

struct job : public function<project()> {
    template <typename T> job(T t) : function<project()>(t) {}
};

project * single(job j) {
    project * p = new project;
    p->j        = new job(std::move(j));
    return p;
}

project fakefib(int n, int d) {
    project p;
    if (n < 0) return p;
    p.deps.push_back(single([n, d]() { return fakefib(n - 2, d + 2); }));
    p.deps.push_back(single([n, d]() { return fakefib(n - 1, d + 2); }));
    p.j = new job([n, d]() {
        for (int i = 0; i < d; ++i) std::cout << ' ';
        std::cout << n << endl;
        return project{};
    });
    return p;
}

project step(project p) {
    if (p.deps.size()) {
        auto pp = step(*(p.deps.back()));
        p.deps.pop_back();
        if ((pp.deps.size() > 0) || (pp.j != nullptr)) p.deps.push_back(new project(pp));
    } else if (p.j) {
        p = (*(p.j))();
    }
    return p;
}

int main() {
    project p = fakefib(4, 0);
    while ((p.deps.size() > 0) || (p.j != nullptr)) p = step(p);
}