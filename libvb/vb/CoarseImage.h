#pragma once /// \file
#include <vb/Bitmap.h>

namespace vb {
    class CoarseCell {
    public:
        explicit CoarseCell(int l = 1) : LL(l * l) {}
        explicit operator Color() { return Grey(fill * 255 / LL); }

        int               fill = 0; ///< The number of pixels with value 1 in the cell.
        std::vector<bool> sub;      ///< The actual contents of the cell, if not constant.
        int               LL;       ///< The number of vertices in the cell.
    };

    class CoarseImage : public Bitmap<CoarseCell> {
    public:
        CoarseImage(int wd, int ht, int l)
            : Bitmap<CoarseCell>(1 + (wd - 1) / l, 1 + (ht - 1) / l, CoarseCell(l)), true_width(wd), true_height(ht), L(l), LL(l * l) {}

        bool contains(coo z) const {
            z += z0;
            return (z.x >= 0) && (z.y >= 0) && (z.x < true_width) && (z.y < true_height);
        }

        bool at(coo z) const;
        void put(coo z, bool c);

        void output_fine(const std::string & fn) const;

        int true_width;  ///< The true width of the image, in pixels.
        int true_height; ///< The true height of the image, in pixels.
        int L;           ///< The size of a block, in pixels.
        int LL;          ///< The square of L
        coo z0 = {0, 0}; ///< The coordinates of the origin (hides that of vb::Bitmap).
    };
} // namespace vb
