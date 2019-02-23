#pragma once
#define BOOST_NO_CXX11_LAMBDAS // missing T(0) in boost::math::tools::polynomial::normalize()
#include <boost/math/tools/polynomial.hpp>
#include <vb/mp.h>
#include <vector>

namespace vb {
    template <typename T> class Polynomial {
    public:
        Polynomial(const boost::math::tools::polynomial<T> &p) : P(p) {}
        Polynomial(const T &t) : P({t}) {}

        auto degree() const { return P.degree(); }

        void add_root(const T &x, int d = 1) {
            for (int i = 0; i < d; ++i) P *= boost::math::tools::polynomial<T>{-x, 1};
        }

        boost::math::tools::polynomial<T> P;
    };

    template <typename T, typename V> V eval(const Polynomial<T> &P, const V &x) {
        if (P.P.size() == 0) return 0;
        V out = P.P[P.degree()];
        for (int i = P.degree() - 1; i >= 0; --i) out = out * x + V{P.P[i]};
        return out;
    }

    template <typename T> Polynomial<T> derivative(const Polynomial<T> &P) {
        if (P.P.size() == 0) return boost::math::tools::polynomial<T>{};
        std::vector<T> out(P.degree());
        for (unsigned i = 0; i < P.degree(); ++i) out[i] = T(i + 1) * P.P[i + 1];
        if (out.empty()) out.push_back(T(0));
        // TODO: fix
        return Polynomial<T>(boost::math::tools::polynomial<T>(begin(out), end(out)));
    }
} // namespace vb

template <typename T> struct fmt::formatter<vb::Polynomial<T>> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const vb::Polynomial<T> &P, FormatContext &ctx) {
        bool               first = true;
        const std::string &v     = "z";
        for (int j = P.degree() + 1; j > 0; --j) {
            int i = j - 1;
            if (P.P[i] == T(0)) continue;
            format_to(ctx.out(), "{}", first ? "" : " + "); // TODO: fmt::join
            if ((i == 0) || (P.P[i] != T(1))) format_to(ctx.out(), "{}", P.P[i]);
            if (i > 0) format_to(ctx.out(), " {}", v);
            if (i > 1) format_to(ctx.out(), "^{}", i);
            first = false;
        }
        if (first) format_to(ctx.out(), "{}", 0);
        return format_to(ctx.out(), "");
    }
};
