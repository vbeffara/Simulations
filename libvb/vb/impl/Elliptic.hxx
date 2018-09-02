#pragma once
#include <vb/Elliptic.h>

namespace vb {
    template <typename T> auto Elliptic<T>::sum(std::function<cplx(int)> f) const -> cplx {
        cplx out(0), old(1);
        for (int n = 0; out != old; ++n) {
            old     = out;
            cplx dd = f(n);
            if (isnormal(real(dd))) out += dd;
        }
        return out;
    }

    template <typename T> Elliptic<T>::Elliptic(cplx q_) {
        q       = std::move(q_);
        q14     = sqrt(sqrt(q));
        eta1_   = eta1();
        eta1_q_ = eta1_q();
        e1_     = e1();
    }

    template <typename T> auto Elliptic<T>::theta1(const cplx & z) const -> cplx {
        return sum([&](int n) {
            cplx out(2);
            out *= pow(T(-1), n);
            out *= q14;
            out *= pow(q, n * (n + 1));
            out *= sin(cplx(2 * n + 1) * z);
            return out;
        });
    }

    template <typename T> auto Elliptic<T>::theta1_z(const cplx & z) const -> cplx {
        return sum([&](int n) {
            cplx out(2);
            out *= pow(cplx(-1), n);
            out *= q14;
            out *= pow(q, n * (n + 1));
            out *= T(2 * n + 1);
            out *= cos(cplx(2 * n + 1) * z);
            return out;
        });
    }

    template <typename T> auto Elliptic<T>::theta1_q(const cplx & z) const -> cplx {
        return sum([&](int n) {
            cplx out(2);
            out *= pow(cplx(-1), n);
            out *= cplx(n) + cplx(1) / cplx(2);
            out *= cplx(n) + cplx(1) / cplx(2);
            out *= q14;
            out *= pow(q, n * (n + 1) - 1);
            out *= sin(cplx(2 * n + 1) * z);
            return out;
        });
    }

    template <typename T> auto Elliptic<T>::theta1_zq(const cplx & z) const -> cplx {
        return sum([&](int n) {
            cplx out(2);
            out *= pow(T(-1), n);
            out *= (n + T(1) / T(2));
            out *= (n + T(1) / T(2));
            out *= q14;
            out *= pow(q, n * (n + 1) - 1);
            out *= T(2 * n + 1);
            out *= cos(cplx(2 * n + 1) * z);
            return out;
        });
    }

    template <typename T> auto Elliptic<T>::theta1_zz(const cplx & z) const -> cplx {
        return sum([&](int n) {
            cplx out(-2);
            out *= q14;
            out *= pow(q, n * (n + 1));
            out *= pow(cplx(2 * n + 1), 2);
            out *= sin(cplx(2 * n + 1) * z);
            return out;
        });
    }

    template <typename T> auto Elliptic<T>::theta1_zzq(const cplx & z) const -> cplx {
        return sum([&](int n) {
            cplx out(-2);
            out *= pow(T(-1), n);
            out *= (n + T(1) / T(2));
            out *= (n + T(1) / T(2));
            out *= q14;
            out *= pow(q, n * (n + 1) - 1);
            out *= pow(T(2 * n + 1), 2);
            out *= sin(cplx(2 * n + 1) * z);
            return out;
        });
    }

    template <typename T> auto Elliptic<T>::theta1_zzz(const cplx & z) const -> cplx {
        return sum([&](int n) {
            cplx out(-2);
            out *= pow(cplx(-1), n);
            out *= q14;
            out *= pow(q, n * (n + 1));
            out *= pow(cplx(2 * n + 1), 3);
            out *= cos(cplx(2 * n + 1) * z);
            return out;
        });
    }

    template <typename T> auto Elliptic<T>::theta1_zzzq(const cplx & z) const -> cplx {
        return sum([&](int n) {
            cplx out(-2);
            out *= pow(T(-1), n);
            out *= cplx(n) + cplx(1) / cplx(2);
            out *= cplx(n) + cplx(1) / cplx(2);
            out *= q14;
            out *= pow(q, n * (n + 1) - 1);
            out *= pow(cplx(2 * n + 1), 3);
            out *= cos(cplx(2 * n + 1) * z);
            return out;
        });
    }

    template <typename T> auto Elliptic<T>::theta2(const cplx & z) const -> cplx {
        return sum([&](int n) {
            cplx out(2);
            out *= q14;
            out *= pow(q, n * (n + 1));
            out *= cos(cplx(2 * n + 1) * z);
            return out;
        });
    }

    template <typename T> auto Elliptic<T>::theta2_z(const cplx & z) const -> cplx {
        return sum([&](int n) {
            cplx out(-2);
            out *= q14;
            out *= pow(q, n * (n + 1));
            out *= cplx(2 * n + 1);
            out *= sin(cplx(2 * n + 1) * z);
            return out;
        });
    }

    template <typename T> auto Elliptic<T>::theta2_q(const cplx & z) const -> cplx {
        return sum([&](int n) {
            cplx out(2);
            out *= cplx(n) + cplx(1) / cplx(2);
            out *= cplx(n) + cplx(1) / cplx(2);
            out *= q14;
            out *= pow(q, n * (n + 1) - 1);
            out *= cos(cplx(2 * n + 1) * z);
            return out;
        });
    }

    template <typename T> auto Elliptic<T>::theta3(const cplx & z) const -> cplx {
        return sum([&](int n) {
            cplx out(n == 0 ? 1 : 2);
            out *= pow(q, n * n);
            out *= cos(cplx(2 * n) * z);
            return out;
        });
    }

    template <typename T> auto Elliptic<T>::theta3_q(const cplx & z) const -> cplx {
        return sum([&](int n) {
            cplx out(2);
            out *= cplx((n + 1) * (n + 1));
            out *= pow(q, n * (n + 2));
            out *= cos(cplx(2 * n + 2) * z);
            return out;
        });
    }

    template <typename T> auto Elliptic<T>::theta4(const cplx & z) const -> cplx {
        return sum([&](int n) {
            cplx out(n == 0 ? 1 : 2);
            out *= pow(-q, n * n);
            out *= cos(T(2 * n) * z);
            return out;
        });
    }

    template <typename T> auto Elliptic<T>::theta4_q(const cplx & z) const -> cplx {
        return sum([&](int n) {
            cplx out(-2);
            out *= T((n + 1) * (n + 1));
            out *= pow(-q, n * (n + 2));
            out *= cos(T(2 * n + 2) * z);
            return out;
        });
    }

    template <typename T> auto Elliptic<T>::eta1() const -> cplx {
        cplx out = -pi_<T>() * pi_<T>() / T(6);
        out *= theta1_zzz(cplx(0));
        out /= theta1_z(cplx(0));
        return out;
    }

    template <typename T> auto Elliptic<T>::eta1_q() const -> cplx {
        cplx out = -pi_<T>() * pi_<T>() / T(6);
        out *= theta1_z(cplx(0)) * theta1_zzzq(cplx(0)) - theta1_zzz(cplx(0)) * theta1_zq(cplx(0));
        out /= pow(theta1_z(cplx(0)), 2);
        return out;
    }

    template <typename T> auto Elliptic<T>::sigma(const cplx & z) const -> cplx {
        return exp(eta1_ * z * z) * theta1(pi_<T>() * z) / (pi_<T>() * theta1_z(cplx(0)));
    }

    template <typename T> auto Elliptic<T>::sigma_q(const cplx & z) const -> cplx {
        cplx pz = pi_<T>() * z;
        return (theta1(pz) * (z * z * eta1_q_ - theta1_zq(cplx(0)) / theta1_z(cplx(0))) + theta1_q(pz)) * exp(eta1_ * z * z) /
               (pi_<T>() * theta1_z(cplx(0)));
    }

    template <typename T> auto Elliptic<T>::e1() const -> cplx {
        cplx out = pi_<T>() * pi_<T>();
        out *= (pow(theta2(cplx(0)), 4) + cplx(2) * pow(theta4(cplx(0)), 4));
        out /= cplx(3);
        return out;
    }

    template <typename T> auto Elliptic<T>::e1_q() const -> cplx {
        cplx out = pi_<T>() * pi_<T>();
        out *= (cplx(4) * theta2_q(cplx(0)) * pow(theta2(cplx(0)), 3) + cplx(8) * theta4_q(cplx(0)) * pow(theta4(cplx(0)), 3));
        out /= cplx(3);
        return out;
    }

    template <typename T> auto Elliptic<T>::zeta(const cplx & z) const -> cplx {
        cplx pz = pi_<T>() * z;
        return T(2) * z * eta1_ + pi_<T>() * theta1_z(pz) / theta1(pz);
    }

    template <typename T> auto Elliptic<T>::zeta_q(const cplx & z) const -> cplx {
        cplx pz = pi_<T>() * z;
        return T(2) * z * eta1_q_ + pi_<T>() * (theta1_zq(pz) - theta1_z(pz) * theta1_q(pz) / theta1(pz)) / theta1(pz);
    }

    template <typename T> auto Elliptic<T>::wp(const cplx & z) const -> cplx {
        cplx pz = pi_<T>() * z;
        return e1_ + pow(pi_<T>() * theta3(cplx(0)) * theta4(cplx(0)) * theta2(pz) / theta1(pz), 2);
    }

    template <typename T> auto Elliptic<T>::wp_z(const cplx & z) const -> cplx {
        cplx pz = pi_<T>() * z;
        return T(2) * pi_<T>() * (theta2_z(pz) / theta2(pz) - theta1_z(pz) / theta1(pz)) * (wp(z) - e1_);
    }

    template <typename T> auto Elliptic<T>::wp_q(const cplx & z) const -> cplx {
        cplx pz = pi_<T>() * z;
        return e1_q() + T(2) * (wp(z) - e1_) *
                            (theta3_q(cplx(0)) / theta3(cplx(0)) + theta4_q(cplx(0)) / theta4(cplx(0)) + theta2_q(pz) / theta2(pz) -
                             theta1_q(pz) / theta1(pz));
    }

    template <typename T> auto Elliptic<T>::g2() const -> cplx {
        cplx a = theta2(cplx(0)), b = theta3(cplx(0));
        return pow(pi_<T>(), 4) / (T(3) / 4) * (pow(a, 8) - pow(a, 4) * pow(b, 4) + pow(b, 8));
    }

    template <typename T> auto Elliptic<T>::g3() const -> cplx {
        cplx a = theta2(cplx(0)), b = theta3(cplx(0));
        return pow(pi_<T>(), 6) / (pow(T(3) / 2, 3)) *
               (pow(a, 12) - (T(3) / 2) * (pow(a, 8) * pow(b, 4) + pow(a, 4) * pow(b, 8)) + pow(b, 12));
    }

    template <typename T> auto Elliptic<T>::j() const -> cplx {
        cplx ll = pow(theta2(cplx(0)) / theta3(cplx(0)), 4);
        return cplx(256) * pow(cplx(1) - ll + ll * ll, 3) / pow(ll * (cplx(1) - ll), 2);
    }
} // namespace vb
