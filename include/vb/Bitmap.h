#pragma once
#include <spdlog/spdlog.h>
#include <vb/Picture.h>
#include <vb/data/Array.h>

namespace vb {
    template <typename T> class Bitmap : public Picture, public Array<T> {
    public:
        Bitmap(const std::string &s, ucoo size, T d = T());

        using Array<T>::size, Array<T>::at, Array<T>::atp, Array<T>::fits;

        void put(const ucoo &z, const T &c) {
            Array<T>::put(z, c);
            step();
        }
        void putp(const coo &z, const T &c) {
            Array<T>::putp(z, c);
            step();
        }

        T lazy(coo z, std::function<T(coo)> f) {
            if (at(z) == dflt) put(z, f(z));
            return at(z);
        }

        void fill(ucoo z, T c, int adj = 4);

    private:
        T dflt; ///< The default value.

    protected:
        void paint() override {
            size_t           ppp = size_t(pixel_w()) / size.x;
            gsl::span<Color> stage((Color *)cairo_image_surface_get_data(surface), gsl::index(ppp * size.x + stride * (ppp * size.y - 1)));

            for (const auto &z : coo_range(size)) {
                for (const auto &dz : coo_range(ucoo{ppp, ppp}))
                    stage[gsl::index(ppp * z.x + dz.x + stride * (ppp * z.y + dz.y))] = to_Color(at(z));
            }
        }
    };

    template <typename T> Bitmap<T>::Bitmap(const std::string &s, ucoo size, T d) : Picture(s, size), Array<T>(size, d), dflt(d) {}

    template <typename T> void Bitmap<T>::fill(ucoo z, T c, int adj) {
        T in = at(z);
        if (in == c) return;
        std::vector<ucoo> xy;
        xy.push_back(z);
        at(z) = c;

        while (!xy.empty()) {
            auto ij = xy.back();
            xy.pop_back();
            for (int d = 0; d < adj; ++d) {
                auto nij = ij + dz[d];
                if (fits(nij) && (at(nij) == in)) {
                    xy.push_back(nij);
                    at(nij) = c;
                }
            }
        }
    }

    using Image = Bitmap<Color>;
} // namespace vb
