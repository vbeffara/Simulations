#pragma once

namespace vb {
    template <typename T> class Wrapped { public:
        Wrapped (const T &tt) : t(tt) {}
        Wrapped () = default;

        operator T () const { return t; }

        operator vb::Color () const;

        bool operator== (const Wrapped<T> &o) const { return t == o.t; }

        T t;
    };
} // namespace vb