#pragma once
#include <optional>
#include <vb/Constellation.h>
#include <vb/Hypermap_lib.h>
#include <vb/math/Polynomial.h>

namespace vb {
    template <typename T> class Constellation0 : public Constellation<T> {
    public:
        using typename Constellation<T>::cplx;
        using Constellation<T>::b;
        using Constellation<T>::w;
        using Constellation<T>::f;
        using Constellation<T>::p;
        using Constellation<T>::findn;
        using Constellation<T>::cost;
        using Constellation<T>::dim;

        Constellation0(const Hypermap &M);
        template <typename U> Constellation0(const Constellation0<U> &C);

        cplx operator()(cplx z) const override;

        std::pair<cplx, cplx> bounds() const override;

        void belyi(); // does too many things at once

        std::optional<Hypermap> explore() const; // Recover the hypermap

    private:
        Vector<cplx> vec() const override;
        void         readvec(const Vector<cplx> &xy) override;

        Vector<cplx> vcost() const override;
        Matrix<cplx> jacvcost() const override;

        cplx logder(cplx z, unsigned k = 0) const;
        void linear(cplx u, cplx v = cplx(0)); // move the points, recompute P and Q, don't touch l
        void normalize();                      // choose l to make ones ones
        void make_l_1();                       // rescale to make l equal to 1
        void make_c_0();                       // shift everybody to make sum(black)=0
        void make_p_1();                       // try to have reasonable scaling
    };

    template <typename T> std::ostream &operator<<(std::ostream &os, const Constellation0<T> &C);
} // namespace vb
