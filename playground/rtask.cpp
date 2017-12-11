#include <vb/ThreadPool.h>
#include <vb/util.h>
#include <mutex>
#include <thread>

using namespace vb;
using namespace std;

Project go(int n) {
    if (n < 2) { return {}; }
    return Project{[=] { return go(n - 1); }, [=] { return go(n - 2); }}.then([=] { return Project{}; });
};

int main(int argc, char ** argv) {
    H.init("Playground for ThreadPool", argc, argv, "n=5");
    execute_par([=] { return go(H['n']); });
}