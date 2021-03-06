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

        Constellation1(const Hypermap &M);
        template <typename U> Constellation1(const Constellation1<U> &C);

        cplx operator()(cplx z) const override;

        std::pair<cplx, cplx> bounds() const override;

        cplx tau() const { return p[0]; }

        int         dx = 0, dy = 0;
        Elliptic<T> E{q_<T>(I_<T>())};

    private:
        Vector<cplx> vec() const override;
        void         readvec(const Vector<cplx> &xy) override;

        Vector<cplx> vcost() const override;
        Matrix<cplx> jacvcost() const override;
        Matrix<cplx> jacnum();

        cplx reduce(cplx z) const override;
        cplx logderp(cplx z, unsigned k) const;
        cplx logderp_z(cplx z, unsigned k) const;
        cplx logderp_q(cplx z, unsigned k) const;
        cplx logderp_t(cplx z, unsigned k) const;
        cplx logder(cplx z, unsigned k) const;
        cplx logder_z(cplx z, unsigned k) const;
        cplx logder_t(cplx z, unsigned k) const;

        void from_points();
        void normalize();
        void shift(cplx z);
    };

    template <typename T> std::ostream &operator<<(std::ostream &os, const Constellation1<T> &C);
} // namespace vb
