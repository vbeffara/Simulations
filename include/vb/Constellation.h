#pragma once
#include <vb/math/LinearAlgebra.h>
#include <vb/util/mp.h>

namespace vb {
    template <typename T> struct Star {
        typename cpx_t<T>::type z;
        size_t                  d;
    };

    template <typename T> class Constellation {
    public:
        using cplx = typename cpx_t<T>::type;

        std::vector<Star<T>> b, w, f;
        std::vector<cplx>    p;

        size_t dim = 0;

        Constellation();
        Constellation(const Constellation &) = default;
        Constellation(Constellation &&)      = default;
        auto operator=(const Constellation &) -> Constellation & = default;
        auto operator=(Constellation &&) -> Constellation & = default;

        template <typename U> explicit Constellation(const Constellation<U> &C);

        virtual ~Constellation() = default;

        [[nodiscard]] virtual auto vec() const -> Vector<cplx>      = 0;
        virtual void               readvec(const Vector<cplx> &xy)  = 0;
        [[nodiscard]] virtual auto vcost() const -> Vector<cplx>    = 0;
        [[nodiscard]] virtual auto jacvcost() const -> Matrix<cplx> = 0;

        virtual auto               operator()(cplx z) const -> cplx = 0;
        [[nodiscard]] virtual auto reduce(cplx z) const -> cplx { return z; }
        [[nodiscard]] virtual auto bounds() const -> std::pair<cplx, cplx> = 0;

        [[nodiscard]] auto cost() const -> T;
        auto               findn() -> T;
    };

    template <> template <> Constellation<real_t>::Constellation(const Constellation<double> &C);
} // namespace vb
