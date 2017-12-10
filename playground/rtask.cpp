#include <vb/ThreadPool.h>
#include <vb/util.h>
#include <mutex>
#include <thread>

using namespace vb;
using namespace std;

Project go(int n, int * t) {
    if (n < 2) {
        *t = 1;
        return {};
    }
    int *   t1 = new int;
    int *   t2 = new int;
    Project p{[=] { return go(n - 1, t1); }, [=] { return go(n - 2, t2); }};
    p.then([=] {
        *t = (*t1) + (*t2);
        delete t1;
        delete t2;
        return Project{};
    });
    return p;
};

int main(int argc, char ** argv) {
    H.init("Playground for ThreadPool", argc, argv, "n=5");
    int * s = new int;
    execute_par([=] { return go(H['n'], s); });
    H.L->info("Fib({}) = {}", int(H['n']), *s);
    delete s;
}