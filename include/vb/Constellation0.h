#pragma once
#include <optional>
#include <vb/Constellation.h>
#include <vb/Hypermap_lib.h>
#include <vb/Polynomial.h>

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

        cplx logder(cplx z, int k = 0) const;
        void linear(cplx u, cplx v = cplx(0)); // move the points, recompute P and Q, don't touch l
        void normalize();                      // choose l to make ones ones
        void make_l_1();                       // rescale to make l equal to 1
        void make_c_0();                       // shift everybody to make sum(black)=0
        void make_p_1();                       // try to have reasonable scaling
    };

    template <typename T> std::ostream &operator<<(std::ostream &os, const Constellation0<T> &C);

#ifdef UNIT_TESTS
    TEST_CASE("vb::Constellation0") {
        // spdlog::set_level(spdlog::level::trace);
        char *                 argv[] = {(char *)"test_constellation0"};
        Hub                    H("Testing Constellation0", 1, argv, "s=3,m=228,d=2,g=0,v,o,b,q");
        auto                   M = HLib().at("m_dodecahedron");
        Constellation0<double> C(M);
        Constellation0<real_t> Cq(C);
        Cq.findn();
        Cq.belyi();
        Cq.belyi();
        Polynomial<complex_t> Q{1};
        for (auto zd : Cq.f)
            for (unsigned j = 0; j < zd.d; ++j) add_root(Q, zd.z);
        // TODO: fix access through P
        for (auto &x : Q.P.data()) {
            auto xx = complex_t(round(real(x)), round(imag(x)));
            if (abs(x - xx) < 1e-90) x = xx;
        }
        CHECK(format(Q) == " z^55 + -55 z^50 + 1205 z^45 + -13090 z^40 + 69585 z^35 + -134761 z^30 + -69585 z^25 + -13090 z^20 + "
                           "-1205 z^15 + -55 z^10 + -1 z^5");
    }
#endif
} // namespace vb
