#define GL_SILENCE_DEPRECATION 1
#include <FL/fl_draw.H>
#include <FL/gl.h>
#include <iomanip>
#include <vb/Picture.h>

namespace vb {
    Picture::Picture(int wd, int ht)
        : AutoWindow(wd, ht), snapshot_prefix(H.title), snapshot_number(0), snapshot_period(0.0), snapshot_task(-1) {}

    Picture::~Picture() {
        if (snapshot_task >= 0) remove_task(snapshot_task);
    }

    void Picture::show() {
        AutoWindow::show();
        if (surface == nullptr) {
            surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, pixel_w(), pixel_h());
            cr      = cairo_create(surface);
            cairo_scale(cr, 1.0 * pixel_w() / w(), 1.0 * pixel_h() / h());
            stride = cairo_image_surface_get_stride(surface) / sizeof(Color);
        }
    }

    void Picture::draw() {
        if (surface == nullptr) {
            surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, pixel_w(), pixel_h());
            cr      = cairo_create(surface);
            cairo_scale(cr, 1.0 * pixel_w() / w(), 1.0 * pixel_h() / h());
            stride = cairo_image_surface_get_stride(surface) / sizeof(Color);
        }
        paint();
        auto sd = static_cast<const ulong *>(static_cast<void *>(cairo_image_surface_get_data(surface)));
        glPixelStorei(GL_UNPACK_ROW_LENGTH, stride);
        glDrawPixels(pixel_w(), pixel_h(), GL_BGRA, GL_UNSIGNED_BYTE, sd);
    }

    void Picture::output_png(const std::string &s) {
        paint();
        std::string os = H.dir + (s.empty() ? H.title : s) + ".png";
        cairo_surface_write_to_png(surface, os.c_str());
    }

    void Picture::output(const std::string &s) { output_png(s); }

    void Picture::output() { output(""); }

    void Picture::snapshot() {
        std::string fn = fmt::format("snapshots/{}_{:04d}", snapshot_prefix, snapshot_number++);
        H.L->info("Taking a snapshot as {}.png", fn);
        output_png(fn);
    }

    void Picture::snapshot_setup(const std::string &prefix, double period) {
        if (snapshot_task >= 0) remove_task(snapshot_task);
        snapshot_period = period;
        snapshot_prefix = prefix;
        snapshot();
        if (period > 0) snapshot_task = add_task(period, [this] { this->snapshot(); });
    }

    int Picture::handle(int event) {
        if ((event == FL_KEYDOWN) && (Fl::event_key() == 's')) snapshot();
        return AutoWindow::handle(event);
    }
} // namespace vb
