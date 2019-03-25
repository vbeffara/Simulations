#pragma once /// \file
#include <vb/Bitmap.h>

namespace vb {
    class CoarseCell {
    public:
        explicit CoarseCell(size_t l = 1) : LL(l * l) {}
        explicit operator Color() { return Grey(uint8_t(fill * 255 / LL)); }

        size_t            fill = 0; ///< The number of pixels with value 1 in the cell.
        std::vector<bool> sub;      ///< The actual contents of the cell, if not constant.
        size_t            LL;       ///< The number of vertices in the cell.
    };

    class CoarseImage : public Bitmap<CoarseCell> {
    public:
        CoarseImage(const std::string &s, ucoo size, size_t l)
            : Bitmap<CoarseCell>(s, {1 + (size.x - 1) / l, 1 + (size.y - 1) / l}, CoarseCell(l)), true_width(size_t(size.x)),
              true_height(size_t(size.y)), L(l), LL(l * l) {}

        // TODO: make this work for ucoo as well
        bool fits(coo z) const {
            z += z0;
            return (z.x >= 0) && (z.y >= 0) && (z.x < int64_t(true_width)) && (z.y < int64_t(true_height));
        }

        // TODO: use ucoo here (get rid of z0)
        bool at(coo z) const;
        void put(coo z, bool c);

        void output_fine(const std::string &fn) const;

        size_t true_width;  ///< The true width of the image, in pixels.
        size_t true_height; ///< The true height of the image, in pixels.
        size_t L;           ///< The size of a block, in pixels.
        size_t LL;          ///< The square of L
        // TODO: get rid of this
        coo z0 = {0, 0}; ///< The coordinates of the origin (hides that of vb::Bitmap).
    };
} // namespace vb
