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
    return Project{[=] { return go(n - 1); }, [=] { return go(n - 2); }}.then([=] { return ++s, Project{}; });
};

int main(int argc, char ** argv) {
    H.init("Playground for ThreadPool", argc, argv, "n=5");
    timing("Plain recursion", [=] {
        s = 1;
        plain(H['n']);
        return int(s);
    });
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
}