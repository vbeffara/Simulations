#pragma once
#include <vb/Constellation.h>
#include <vb/Elliptic.h>
#include <vb/Hypermap.h>
#include <vb/math/NumberTheory.h>

namespace vb {
    template <typename T> class Constellation1 : public Constellation<T> {
    public:
        using typename Constellation<T>::cplx;
        using Constellation<T>::b;
        using Constellation<T>::w;
        using Constellation<T>::f;
        using Constellation<T>::p;
        using Constellation<T>::findn;
        using Constellation<T>::cost;
        using Constellation<T>::dim;

        explicit Constellation1(const Hypermap &M);
        template <typename U> explicit Constellation1(const Constellation1<U> &C);

        auto operator()(cplx z) const -> cplx override;

        auto bounds() const -> std::pair<cplx, cplx> override;

        auto tau() const -> cplx { return p[0]; }

        int         dx = 0, dy = 0;
        Elliptic<T> E{q_<T>(I_<T>())};

    private:
        auto         vec() const -> Vector<cplx> override;
        void         readvec(const Vector<cplx> &xy) override;

        auto vcost() const -> Vector<cplx> override;
        auto jacvcost() const -> Matrix<cplx> override;
        auto jacnum() -> Matrix<cplx>;

        auto reduce(cplx z) const -> cplx override;
        auto logderp(cplx z, unsigned k) const -> cplx;
        auto logderp_z(cplx z, unsigned k) const -> cplx;
        auto logderp_q(cplx z, unsigned k) const -> cplx;
        auto logderp_t(cplx z, unsigned k) const -> cplx;
        auto logder(cplx z, unsigned k) const -> cplx;
        auto logder_z(cplx z, unsigned k) const -> cplx;
        auto logder_t(cplx z, unsigned k) const -> cplx;

        void from_points();
        void normalize();
        void shift(cplx z);
    };

    template <typename T> auto operator<<(std::ostream &os, const Constellation1<T> &C) -> std::ostream &;
} // namespace vb
