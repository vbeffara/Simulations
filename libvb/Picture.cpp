#define GL_SILENCE_DEPRECATION 1
#include <FL/fl_draw.H>
#include <FL/gl.h>
#include <vb/Picture.h>

namespace vb {
    Picture::Picture(const std::string &s, ucoo size) : AutoWindow(s, size), snapshot_prefix(s), snapshot_number(0), snapshot_period(0.0) {}

    Picture::~Picture() {
        if (snapshot_task) remove_task(*snapshot_task);
    }

    void Picture::show() {
        AutoWindow::show();
        if (surface == nullptr) {
            surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, pixel_w(), pixel_h());
            context = cairo_create(surface);
            cairo_scale(context, 1.0 * pixel_w() / w(), 1.0 * pixel_h() / h());
            stride = size_t(cairo_image_surface_get_stride(surface)) / sizeof(Color);
        }
    }

    void Picture::draw() {
        if (surface == nullptr) {
            surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, pixel_w(), pixel_h());
            context = cairo_create(surface);
            cairo_scale(context, 1.0 * pixel_w() / w(), 1.0 * pixel_h() / h());
            stride = size_t(cairo_image_surface_get_stride(surface)) / sizeof(Color);
        }
        paint();
        const auto *sd = static_cast<const ulong *>(static_cast<void *>(cairo_image_surface_get_data(surface)));
        glPixelStorei(GL_UNPACK_ROW_LENGTH, GLint(stride));
        glDrawPixels(pixel_w(), pixel_h(), GL_BGRA, GL_UNSIGNED_BYTE, sd);
    }

    void Picture::output_png(const std::string &s) {
        paint();
        std::string const os = s + ".png";
        cairo_surface_write_to_png(surface, os.c_str());
    }

    void Picture::output(const std::string &s) { output_png(s); }

    void Picture::snapshot() {
        std::string const fn = fmt::format("snapshots/{}_{:04d}", snapshot_prefix, snapshot_number++);
        output_png(fn);
    }

    void Picture::snapshot_setup(const std::string &prefix, double per) {
        if (snapshot_task) remove_task(*snapshot_task);
        snapshot_period = per;
        snapshot_prefix = prefix;
        snapshot();
        if (per > 0) snapshot_task = add_task(per, [this] { this->snapshot(); });
    }

    auto Picture::handle(int event) -> int {
        if ((event == FL_KEYDOWN) && (Fl::event_key() == 's')) snapshot();
        return AutoWindow::handle(event);
    }
} // namespace vb
