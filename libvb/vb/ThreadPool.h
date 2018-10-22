#pragma once
#include <vb/Hub.h>
#include <boost/lockfree/stack.hpp>
#include <functional>

namespace vb {
    struct Context {
        struct Will : public std::function<void()> {
            using function::function;
            ~Will() { (*this)(); }
        };

        boost::lockfree::stack<std::function<void()>> &S;
        std::shared_ptr<Will>                          next;

        void then(std::function<void(Context)> f);
        void push(std::function<void(Context)> f);
    };

    void run_par(const std::function<void(Context)> &f);

    void loop_go(Context C, int a, int b, std::function<void(int)> f, int l);
    void loop_par(int a, int b, std::function<void(int)> f, int l = 100);
} // namespace vb
