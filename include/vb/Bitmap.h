#pragma once
#include <vb/Picture.h>
#include <vb/data/Array.h>

namespace vb {
    template <typename T> class Bitmap : public Picture, public Array<T> {
    public:
        Bitmap(const std::string &s, ucoo size, T d = T());

        using Array<T>::size;
        using Array<T>::at;
        using Array<T>::atp;
        using Array<T>::contains; // TODO: rename, clashes with Fl_Widget

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

        void fill(coo z, T c, int adj = 4);

    private:
        T dflt; ///< The default value.

    protected:
        void paint() override {
            auto             ppp = size_t(pixel_w() / w());
            gsl::span<Color> stage((Color *)cairo_image_surface_get_data(surface),
                                   gsl::index(ppp * size_t(w()) + stride * (ppp * size_t(h()) - 1)));

            for (size_t x = 0; x < size_t(w()); ++x)
                for (size_t y = 0; y < size_t(h()); ++y)
                    for (size_t dx = 0; dx < ppp; ++dx)
                        for (size_t dy = 0; dy < ppp; ++dy)
                            stage[gsl::index(ppp * x + dx + stride * (ppp * y + dy))] = to_Color(at({x, y}));
        }
    };

    template <typename T> Bitmap<T>::Bitmap(const std::string &s, ucoo size, T d) : Picture(s, size), Array<T>(size, d), dflt(d) {}

    template <typename T> void Bitmap<T>::fill(coo z, T c, int adj) {
        T in = at(z);
        if (in == c) return;
        std::vector<coo> xy;
        xy.push_back(z);
        at(z) = c;

        while (xy.size()) {
            coo ij = xy.back();
            xy.pop_back();
            for (int d = 0; d < adj; ++d) {
                coo nij = ij + dz[d];
                if (contains(nij) && (at(nij) == in)) {
                    xy.push_back(nij);
                    at(nij) = c;
                }
            }
        }
    }

    using Image = Bitmap<Color>;

#ifdef UNIT_TESTS
    TEST_CASE("vb::Image") {
        char *argv[] = {(char *)"test_bitmap"};
        Hub   H("Testing Image", 1, argv);
        Image img(H.title, {256, 256});
        img.show();
        for (size_t i = 0; i < 256; ++i)
            for (size_t j = 0; j < 256; ++j) img.put({i, j}, Color(uint8_t(i), uint8_t(j), uint8_t((8 * (i + j)) % 256)));
        img.hide();
    }
#endif
} // namespace vb
