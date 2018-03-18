#include <vb/CoarseImage.h>
#include <cstring>
#include <png++/png.hpp>

namespace vb {
    bool CoarseImage::at(coo z) const {
        z += z0;
        const CoarseCell & d = Bitmap<CoarseCell>::at(coo(z.x / L, z.y / L));
        if (d.fill == 0) return 0;
        if (d.fill == LL) return 1;
        return d.sub[(z.x % L) + L * (z.y % L)] > 0 ? true : false;
    }

    void CoarseImage::put(coo z, bool c) {
        step();
        z += z0;
        CoarseCell & d = Bitmap<CoarseCell>::at(coo(z.x / L, z.y / L));
        if (d.fill == c * LL) return;
        if (d.fill == (1 - c) * LL) d.sub.resize(LL, 1 - c);
        int sub_xy = (z.x % L) + L * (z.y % L);
        if (d.sub[sub_xy] != c) {
            d.sub[sub_xy] = c;
            d.fill += 2 * c - 1;
        }
        if ((d.fill == 0) || (d.fill == LL)) d.sub.shrink_to_fit();
    }

    void CoarseImage::output_fine(const std::string & fn) const {
        using row = png::packed_pixel_row<png::gray_pixel_1>;

        png::image_info m_info(png::make_image_info<png::gray_pixel_1>());
        m_info.set_width(true_width);
        m_info.set_height(true_height);

        std::ofstream             stream(fn, std::ios::binary);
        png::writer<std::ostream> wr(stream);
        wr.set_image_info(m_info);
        wr.write_info();

        row m_row(true_width);
        for (int j = 0; j < true_height; ++j) {
            for (int i = 0; i < true_width; ++i) m_row[i] = !at(coo(i, j) - z0);
            wr.write_row(png::row_traits<row>::get_data(m_row));
        }

        wr.write_end_info();
    }
} // namespace vb
