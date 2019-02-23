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

        boost::math::tools::polynomial<T> P;
    };

    template <typename T> void add_root(Polynomial<T> &P, const T &x, int d = 1) {
        for (int i = 0; i < d; ++i) P.P *= boost::math::tools::polynomial<T>{-x, 1};
    }

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

    template <typename T> std::string format(const Polynomial<T> &P, const std::string &v = "z") {
        std::ostringstream os;
        bool               first = true;
        for (int j = P.degree() + 1; j > 0; --j) {
            int i = j - 1;
            if (P.P[i] == T(0)) continue;
            os << (first ? "" : " + ");
            if ((i == 0) || (P.P[i] != T(1))) os << P.P[i];
            if (i > 0) os << " " << v;
            if (i > 1) os << "^" << i;
            first = false;
        }
        if (first) os << 0;
        return os.str();
    }
} // namespace vb

// TODO: extend that to general coefficient types (maybe needs to change the Polynomial class)
template <> struct fmt::formatter<vb::Polynomial<vb::mpz_int>> {
    template <typename ParseContext> constexpr auto parse(ParseContext &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const vb::Polynomial<vb::mpz_int> &P, FormatContext &ctx) {
        // TODO: transfer the format code here
        return format_to(ctx.out(), "{}", vb::format(P));
    }
};
