#include <cmath>
#include <functional>
#include <iostream>
#include <optional>
#include <vb/Hub.h>
#include <vector>

using namespace std;
using vb::H;

struct task : public function<vector<task>(void)> {
    using function<vector<task>(void)>::function;
};

class thread_pool {
public:
    vector<task>   tasks;
    vector<thread> runners;
    mutex          tasks_m;
    atomic<int>    running = 0;
    bool           stop    = false;

    void enqueue(task t) {
        lock_guard l(tasks_m);
        tasks.push_back(t);
    }

    void runner() {
        task t;
        while (!(stop && tasks.empty() && (running == 0))) {
            {
                lock_guard l(tasks_m);
                if (tasks.empty()) continue;
                t = tasks.back();
                ++running;
                tasks.pop_back();
            }
            for (auto tt : t()) enqueue(tt);
            --running;
        }
    }

    thread_pool() {
        int nt = thread::hardware_concurrency();
        for (int i = 0; i < (nt ? nt : 1); ++i) runners.push_back(thread([=] { runner(); }));
    }

    ~thread_pool() {
        stop = true;
        for (auto & t : runners) t.join();
    }
};

double cost(double x) {
    for (int i = 0; i < 10; ++i) x = cos(x);
    return x;
}

vector<task> go(vector<double> & X, int i, int j) {
    if (j - i <= 10000) {
        for (int k = i; k < j; ++k) X[k] = cost(k);
        return {};
    }
    int k = (i + j) / 2;
    return {bind(go, ref(X), i, k), bind(go, ref(X), k, j)};
}

int main(int argc, char ** argv) {
    H.init("Thread pool project", argc, argv, "l=4000000");
    vector<double> X((int(H['l'])));

    thread_pool().enqueue([&X] { return go(X, 0, X.size()); });

    double s = 0;
    for (auto x : X) s += x;
    cout << s - int(s) << '\n';
}