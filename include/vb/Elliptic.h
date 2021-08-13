#pragma once
#include <vb/math/math.h>

namespace vb {
    template <typename T> class Elliptic {
    public:
        using cplx = typename cpx_t<T>::type;

        explicit Elliptic(cplx q_);

        auto eta1() const -> cplx;
        auto eta1_q() const -> cplx;
        auto e1() const -> cplx;
        auto e1_q() const -> cplx;
        auto g2() const -> cplx;
        auto g3() const -> cplx;
        auto j() const -> cplx;

        auto theta1(const cplx &z) const -> cplx;
        auto theta1_z(const cplx &z) const -> cplx;
        auto theta1_zz(const cplx &z) const -> cplx;
        auto theta1_zzz(const cplx &z) const -> cplx;
        auto theta1_q(const cplx &z) const -> cplx;
        auto theta1_zq(const cplx &z) const -> cplx;
        auto theta1_zzq(const cplx &z) const -> cplx;
        auto theta1_zzzq(const cplx &z) const -> cplx;
        auto theta2(const cplx &z) const -> cplx;
        auto theta2_z(const cplx &z) const -> cplx;
        auto theta2_q(const cplx &z) const -> cplx;
        auto theta3(const cplx &z) const -> cplx;
        auto theta3_q(const cplx &z) const -> cplx;
        auto theta4(const cplx &z) const -> cplx;
        auto theta4_q(const cplx &z) const -> cplx;

        auto sigma(const cplx &z) const -> cplx;
        auto sigma_q(const cplx &z) const -> cplx;
        auto zeta(const cplx &z) const -> cplx;
        auto zeta_q(const cplx &z) const -> cplx;
        auto wp(const cplx &z) const -> cplx;
        auto wp_z(const cplx &z) const -> cplx;
        auto wp_q(const cplx &z) const -> cplx;

        cplx q, Pi;

        cplx q14, eta1_, eta1_q_, e1_;
    };
} // namespace vb
