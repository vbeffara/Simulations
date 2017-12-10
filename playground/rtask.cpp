#include <vb/ThreadPool.h>
#include <iostream>
#include <mutex>
#include <thread>

using vb::Project;

Project fakefib(int n, int d) {
    Project p;
    if (n > 0) p.add([=] { return fakefib(n - 1, d + 2); });
    if (n > 1) p.add([=] { return fakefib(n - 2, d + 2); });
    p.then([n, d]() -> Project {
        for (int i = 0; i < d; ++i) std::cout << ' ';
        std::cout << n << '\n';
        return {};
    });
    return p;
}

int main() {
    vb::execute_par([] { return fakefib(5, 0); });
}