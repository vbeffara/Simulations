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

        template <typename V> V operator()(const V &x) const {
            if (P.size() == 0) return 0;
            V out = P[degree()];
            for (int i = degree() - 1; i >= 0; --i) out = out * x + V{P[i]};
            return out;
        }

        Polynomial<T> derivative() const {
            if (P.size() <= 1) return T{0};
            std::vector<T> out(P.degree());
            for (unsigned i = 0; i < P.degree(); ++i) out[i] = T(i + 1) * P[i + 1];
            return boost::math::tools::polynomial<T>(begin(out), end(out));
        }

        void add_root(const T &x, int d = 1) {
            for (int i = 0; i < d; ++i) P *= boost::math::tools::polynomial<T>{-x, 1};
        }

        boost::math::tools::polynomial<T> P;
    };
} // namespace vb

template <typename T> struct fmt::formatter<vb::Polynomial<T>> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const vb::Polynomial<T> &P, FormatContext &ctx) {
        std::vector<std::string> monomials;
        for (int i = P.degree(); i >= 0; --i) {
            if (P.P[i] == T(0)) continue;
            if (i == 0)
                monomials.push_back(fmt::format("{}", P.P[i]));
            else {
                std::string s = "z";
                if (P.P[i] != T(1)) s = fmt::format("{} ", P.P[i]) + s;
                if (i > 1) s = s + fmt::format("^{}", i);
                monomials.push_back(s);
            }
        }
        if (monomials.empty()) return format_to(ctx.out(), "{}", 0);
        return format_to(ctx.out(), "{}", fmt::join(monomials, " + "));
    }
};
