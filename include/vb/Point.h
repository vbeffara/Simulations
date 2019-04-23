#pragma once
#include <queue>
#include <vb/util/coo.h>

namespace vb {
    template <typename T> struct Point : public T {
        Point(T z, double t) : T(z), t(t) {}
        bool   operator<(const Point &pt) const { return t > pt.t; }
        double t = 0;
    };

    template <typename T> struct PointQueue : public std::priority_queue<Point<T>> {
        auto get() {
            Point<T> p{std::priority_queue<Point<T>>::top()};
            std::priority_queue<Point<T>>::pop();
            return p;
        }
    };
} // namespace vb
