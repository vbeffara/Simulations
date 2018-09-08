#include <vb/CoarseImage.h>
#include <fstream>
#include <png.h>

static void write_data(png_struct * png, uint8_t * data, size_t length) {
    auto stream = static_cast<std::ostream *>(png_get_io_ptr(png));
    stream->write(static_cast<char *>(static_cast<void *>(data)), length);
}

static void flush_data(png_struct * png) {
    auto stream = static_cast<std::ostream *>(png_get_io_ptr(png));
    stream->flush();
}

namespace vb {
    bool CoarseImage::at(coo z) const {
        z += z0;
        const CoarseCell & d = Bitmap<CoarseCell>::at({z.x / L, z.y / L});
        if (d.fill == 0) return false;
        if (d.fill == LL) return true;
        return d.sub[(z.x % L) + L * (z.y % L)];
    }

    void CoarseImage::put(coo z, bool c) {
        step();
        z += z0;
        int          cc = c ? 1 : 0;
        CoarseCell & d  = Bitmap<CoarseCell>::at({z.x / L, z.y / L});
        if (d.fill == cc * LL) return;
        if (d.fill == (1 - cc) * LL) d.sub.resize(LL, !c);
        int sub_xy = (z.x % L) + L * (z.y % L);
        if (d.sub[sub_xy] != c) {
            d.sub[sub_xy] = c;
            d.fill += 2 * cc - 1;
        }
        if ((d.fill == 0) || (d.fill == LL)) d.sub.shrink_to_fit();
    }

    void CoarseImage::output_fine(const std::string & fn) const {
        auto stream = std::ofstream(fn, std::ios::binary);
        auto png    = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
        auto info   = png_create_info_struct(png);

        png_set_write_fn(png, &stream, write_data, flush_data);
        png_set_IHDR(png, info, true_width, true_height, 1, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
                     PNG_FILTER_TYPE_DEFAULT);
        png_write_info(png, info);

        for (int j = 0; j < true_height; ++j) {
            auto row = std::vector<uint8_t>(1 + true_width / 8, 0);
            for (int i = 0; i < true_width; ++i)
                if (!at(coo{i, j} - z0)) row.at(i / 8) |= (128u >> static_cast<unsigned>(i % 8));
            png_write_row(png, row.data());
        }

        png_write_end(png, info);
        png_destroy_info_struct(png, &info);
        png_destroy_write_struct(&png, &info);
    }
} // namespace vb
