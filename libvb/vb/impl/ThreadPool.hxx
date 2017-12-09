#pragma once
#include <vb/ThreadPool.h>
#include <mutex>
#include <thread>

namespace vb {
    template <typename T> Project2 & Project2::add(T t) {
        ++ndep;
        deps.emplace_back();
        deps.back().next = std::move(t);
        deps.back().par  = this;
        return *this;
    }

    template <typename T> Project2 & Project2::then(T t) {
        next = std::move(t);
        return *this;
    }
} // namespace vb