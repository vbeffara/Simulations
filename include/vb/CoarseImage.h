#pragma once /// \file
#include <vb/Bitmap.h>

namespace vb {
    class CoarseCell : public std::vector<bool> {
    public:
        explicit CoarseCell(size_t l) : LL(l * l) {}
        explicit operator Color() const { return Grey(uint8_t(fill * 255 / LL)); }

        size_t fill = 0; ///< The number of pixels with value 1 in the cell.
        size_t LL;       ///< The number of vertices in the cell.
    };

    class CoarseImage : public Bitmap<CoarseCell> {
    public:
        CoarseImage(const std::string &s, ucoo sz, size_t l)
            : Bitmap<CoarseCell>(s, {1 + (sz.x - 1) / l, 1 + (sz.y - 1) / l}, CoarseCell(l)), size(sz), L(l), LL(l * l) {}

        CoarseImage(const std::string &s, ucoo sz) : CoarseImage(s, sz, size_t(pow(double(sup(sz)), .33))) {}

        template <typename T> auto fits(const coo_2d<T> &z) const -> bool {
            return (z.x >= 0) && (z.y >= 0) && (z.x < T(size.x)) && (z.y < T(size.y));
        }

        [[nodiscard]] auto at(ucoo z) const -> bool;
        void               put(ucoo z, bool c);

        void output_fine(const std::string &fn) const;

        ucoo size;

        size_t L;  ///< The size of a block, in pixels.
        size_t LL; ///< The square of L
    };
} // namespace vb
