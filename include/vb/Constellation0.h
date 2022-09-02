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

        explicit Constellation0(const Hypermap &M);
        template <typename U> explicit Constellation0(const Constellation0<U> &C);

        auto operator()(cplx z) const -> cplx override;

        auto bounds() const -> std::pair<cplx, cplx> override;

        void belyi(); // does too many things at once

        [[nodiscard]] auto explore() const -> std::optional<Hypermap>; // Recover the hypermap

    private:
        auto vec() const -> Vector<cplx> override;
        void readvec(const Vector<cplx> &xy) override;

        auto vcost() const -> Vector<cplx> override;
        auto jacvcost() const -> Matrix<cplx> override;

        auto logder(cplx z, unsigned k = 0) const -> cplx;
        void linear(cplx u, cplx v = cplx(0)); // move the points, recompute P and Q, don't touch l
        void normalize();                      // choose l to make ones ones
        void make_l_1();                       // rescale to make l equal to 1
        void make_c_0();                       // shift everybody to make sum(black)=0
        void make_p_1();                       // try to have reasonable scaling
    };

    template <typename T> auto operator<<(std::ostream &os, const Constellation0<T> &C) -> std::ostream &;
} // namespace vb

template <typename T> struct fmt::formatter<vb::Constellation0<T>> : fmt::ostream_formatter {}; // TODO: do this better
