#include <cmath>
#include <functional>
#include <future>
#include <iostream>
#include <optional>
#include <vb/Hub.h>
#include <vector>

using namespace std;
using vb::H;

class thread_pool {
public:
    vector<function<void(void)>> tasks;
    vector<future<void>>         runners;
    mutex                        tasks_m;
    atomic<int>                  running = 0;
    bool                         stop    = false;

    void enqueue(function<void(void)> t) {
        lock_guard l(tasks_m);
        tasks.push_back(t);
    }

    void runner() {
        function<void(void)> t;
        while (!(stop && tasks.empty() && (running == 0))) {
            {
                lock_guard l(tasks_m);
                if (tasks.empty()) continue;
                t = tasks.back();
                ++running;
                tasks.pop_back();
            }
            t();
            --running;
        }
    }

    thread_pool() {
        int nt = thread::hardware_concurrency();
        for (int i = 0; i < (nt ? nt : 1); ++i) runners.emplace_back(async([=] { runner(); }));
    }

    ~thread_pool() { stop = true; }
};

double cost(double x) {
    for (int i = 0; i < 10; ++i) x = cos(x);
    return x;
}

void go(thread_pool & TP, vector<double> & X, int i, int j) {
    if (j - i <= 1000) {
        for (int k = i; k < j; ++k) X[k] = cost(k);
        return;
    }
    int k = (i + j) / 2;
    TP.enqueue([k, j, &X, &TP] { go(TP, X, k, j); });
    TP.enqueue([i, k, &X, &TP] { go(TP, X, i, k); });
}

int main(int argc, char ** argv) {
    H.init("Thread pool project", argc, argv, "l=4000000");
    vector<double> X((int(H['l'])));

    {
        thread_pool TP;
        TP.enqueue([&X, &TP] { go(TP, X, 0, X.size()); });
    }

    double s = 0;
    for (auto x : X) s += x;
    cout << s - int(s) << '\n';
}