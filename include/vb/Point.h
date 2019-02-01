#pragma once
#include <queue>
#include <vb/util/coo.h>

namespace vb {
    struct Point : public coo {
        Point(coo z, double t) : coo(z), t(t) {}
        bool   operator<(const Point &pt) const { return t > pt.t; }
        double t = 0;
    };

    struct PointQueue : public std::priority_queue<Point> {
        auto get() {
            Point p{top()};
            pop();
            return p;
        }
    };
} // namespace vb
