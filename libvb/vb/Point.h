#pragma once
#include <vb/coo.h>
#include <queue>

namespace vb {
	struct Point : public coo {
		bool operator< (const Point &pt) const { return t > pt.t; };
		double t=0;
	};

	struct PointQueue : public std::priority_queue<Point> {
		auto get () { auto p {top()}; pop(); return p; }
	};
}
