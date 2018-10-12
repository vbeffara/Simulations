#pragma once
#include <vb/Hub.h>
#include <atomic>
#include <boost/lockfree/stack.hpp>
#include <functional>
#include <optional>
#include <vector>

namespace vb {
    using Work = std::function<void()>;
    using BLST = boost::lockfree::stack<Work>;

    struct Will : public Work {
        using Work::Work;
        ~Will() { (*this)(); }
    };

    void run_par(const std::function<void(BLST &, std::shared_ptr<Will>)> &f);

    void loop_go(BLST &S, std::shared_ptr<Will> parent, int a, int b, std::function<void(int)> f, int l);
    void loop_par(int a, int b, std::function<void(int)> f, int l = 100);
} // namespace vb
