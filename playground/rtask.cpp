#include <vb/ThreadPool.h>
#include <vb/util.h>

using namespace vb;
using namespace std;

atomic<int> s;

void plain(int n) {
    if (n < 2) return;
    plain(n - 1);
    plain(n - 2);
    ++s;
};

Project go(int n) {
    if (n < 2) { return {}; }
    Project p{[=] { return go(n - 1); }, [=] { return go(n - 2); }, [=] { return ++s, Project{}; }};
    // p.then([=] { return ++s, Project{}; });
    return p;
};

int main(int argc, char ** argv) {
    H.init("Playground for ThreadPool", argc, argv, "n=5,r");
    timing("ThreadPool (execute_run)", [=] {
        s = 1;
        execute_run([=] { return go(H['n']); });
        return int(s);
    });
    if (H['r']) return 0;
    timing("ThreadPool (execute_seq)", [=] {
        s = 1;
        execute_seq([=] { return go(H['n']); });
        return int(s);
    });
    timing("ThreadPool (execute_par)", [=] {
        s = 1;
        execute_par([=] { return go(H['n']); });
        return int(s);
    });
    timing("Plain recursion", [=] {
        s = 1;
        plain(H['n']);
        return int(s);
    });
}