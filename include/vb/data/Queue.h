#pragma once
#include <queue>

namespace vb {
    template <typename T> struct Queue {
        struct Item : public T {
            Item(T z, double t) : T(z), t(t) {}
            bool   operator<(const Item &pt) const { return t > pt.t; }
            double t = 0;
        };

        std::priority_queue<Item> q;

        void push(Item i) { q.push(i); }

        auto get() {
            Item p{q.top()};
            q.pop();
            return p;
        }
    };
} // namespace vb
