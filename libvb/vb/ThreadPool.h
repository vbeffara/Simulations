#pragma once
#include <vb/Hub.h>
#include <boost/lockfree/stack.hpp>
#include <functional>
#include <vector>

namespace vb {
    using Work = std::function<void()>;

    struct Will : public Work {
        using Work::Work;
        ~Will() { (*this)(); }
    };

    struct Context {
        boost::lockfree::stack<Work> &S;
        std::shared_ptr<Will>         next;
    };

    void run_par(const std::function<void(Context)> &f);

    void loop_go(Context C, int a, int b, std::function<void(int)> f, int l);
    void loop_par(int a, int b, std::function<void(int)> f, int l = 100);
} // namespace vb
