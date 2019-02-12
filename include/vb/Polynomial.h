#pragma once
#define BOOST_NO_CXX11_LAMBDAS // missing T(0) in boost::math::tools::polynomial::normalize()
#include <boost/math/tools/polynomial.hpp>
#include <vector>

namespace vb {
    template <typename T> using Polynomial = boost::math::tools::polynomial<T>;

    template <typename T> void add_root(Polynomial<T> &P, const T &x, int d = 1) {
        for (int i = 0; i < d; ++i) P *= Polynomial<T>{-x, 1};
    }

    template <typename T, typename V> V eval(const Polynomial<T> &P, const V &x) {
        if (P.size() == 0) return 0;
        V out = P[P.degree()];
        for (int i = P.degree() - 1; i >= 0; --i) out = out * x + V{P[i]};
        return out;
    }

    template <typename T> Polynomial<T> derivative(const Polynomial<T> &P) {
        if (P.size() == 0) return {};
        std::vector<T> out(P.degree());
        for (unsigned i = 0; i < P.degree(); ++i) out[i] = T(i + 1) * P[i + 1];
        if (out.empty()) out.push_back(T(0));
        return Polynomial<T>(begin(out), end(out));
    }

    template <typename T> std::string format(const Polynomial<T> &P, const std::string &v = "z") {
        std::ostringstream os;
        bool               first = true;
        for (int j = P.degree() + 1; j > 0; --j) {
            int i = j - 1;
            if (P[i] == T(0)) continue;
            os << (first ? "" : " + ");
            if ((i == 0) || (P[i] != T(1))) os << P[i];
            if (i > 0) os << " " << v;
            if (i > 1) os << "^" << i;
            first = false;
        }
        if (first) os << 0;
        return os.str();
    }
} // namespace vb
