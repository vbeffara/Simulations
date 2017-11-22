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

    bool done() { return (tasks.empty()) && (running == 0); }

    void enqueue(function<void(void)> t) {
        unique_lock l(tasks_m);
        tasks.push_back(t);
    }

    optional<function<void(void)>> get_one() {
        unique_lock l(tasks_m);
        if (tasks.empty()) return {};
        auto t = tasks.back();
        ++running;
        tasks.pop_back();
        return t;
    }

    void runner() {
        while (!done()) {
            if (auto t = get_one()) {
                (*t)();
                --running;
            }
        }
    }

    void start() {
        for (int i = 0; i < 10; ++i) runners.emplace_back(async([=] { runner(); }));
    }

    void finish() { runners.clear(); }
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
    H.init("Thread pool project", argc, argv, "n=41,l=4000000");
    int n = H['n'], l = H['l'];

    vector<double> X(l);

    thread_pool TP;
    TP.enqueue([l, &X, &TP] { go(TP, X, 0, l); });
    TP.start();
    TP.finish();

    double s = 0;
    for (auto x : X) s += x;
    cout << s - int(s) << '\n';
}