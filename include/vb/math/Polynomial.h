#pragma once
#include <boost/math/tools/polynomial.hpp>
#include <fmt/ostream.h>

namespace vb {
    template <typename T> class Polynomial {
    public:
        Polynomial(const boost::math::tools::polynomial<T> &p) : P(p) {}
        Polynomial(const std::vector<T> &V) : P(begin(V), end(V)) {}
        Polynomial(const T &t) : P({t}) {}

        size_t degree() const { return P.degree(); }
        size_t size() const { return P.size(); }

        T  operator[](size_t i) const { return P[i]; }
        T &operator[](size_t i) { return P[i]; }

        template <typename V> V operator()(const V &x) const {
            if (P.size() == 0) return 0;
            V out = P[degree()];
            if (degree() == 0) return out;
            for (auto i = degree() - 1; i <= degree() - 1; --i) out = out * x + V{P[i]};
            return out;
        }

        Polynomial<T> derivative() const {
            if (P.size() <= 1) return T{0};
            std::vector<T> out(P.degree());
            for (unsigned i = 0; i < P.degree(); ++i) out[i] = T(i + 1) * P[i + 1];
            return out;
        }

        void operator*=(const T &x) { P *= x; }

        void add_root(const T &x, size_t d = 1) {
            for (size_t i = 0; i < d; ++i) P *= boost::math::tools::polynomial<T>{-x, 1};
        }

    private:
        boost::math::tools::polynomial<T> P;
    };
} // namespace vb

template <typename T> struct fmt::formatter<vb::Polynomial<T>> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const vb::Polynomial<T> &P, FormatContext &ctx) {
        std::vector<std::string> monomials;
        for (auto i = P.degree(); i <= P.degree(); --i) {
            if (P[i] == T(0)) continue;
            if (i == 0)
                monomials.push_back(fmt::format("{}", P[i]));
            else {
                std::string s = "z";
                if (P[i] != T(1)) s = fmt::format("{} ", P[i]) + s;
                if (i > 1) s = s + fmt::format("^{}", i);
                monomials.push_back(s);
            }
        }
        if (monomials.empty()) return format_to(ctx.out(), "{}", 0);
        return format_to(ctx.out(), "{}", fmt::join(monomials, " + "));
    }
};
