#pragma once
#include <vb/Color.h>

namespace vb {
    template <typename T> Color to_Color (T t);

    template <typename T> class Wrapped { public:
        Wrapped (const T &tt) : t(tt) {}
        Wrapped () = default;

        operator T () const { return t; }

        operator vb::Color () const { return to_Color (t); }

        bool operator== (const Wrapped<T> &o) const { return t == o.t; }

        T t;
    };
} // namespace vb