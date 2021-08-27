#pragma once /// \file
#include <FL/Fl_Gl_Window.H>
#include <cairo/cairo.h>
#include <optional>
#include <vb/AutoWindow.h>
#include <vb/Color.h>

namespace vb {
    /** A nice helper class for simulations.
     *
     * Subclass of AutoWindow to manage actual pictures (bitmaps, figures
     * and so on) as opposed to more FLTK-like windows.
     */

    class Picture : public AutoWindow<Fl_Gl_Window> {
    public:
        Picture(const std::string &s, ucoo size);
        ~Picture() override;

        Picture(const Picture &) = delete;
        Picture(Picture &&)      = delete;
        auto operator=(const Picture &) -> Picture & = delete;
        auto operator=(Picture &&) -> Picture & = delete;

        /// Output the image in the preferred format (PNG by default).
        virtual void output(const std::string &s);
        void         output_png(const std::string &s);

        void snapshot_setup(const std::string &prefix, double per = 0.0);
        void snapshot();

        void show() override;
        auto handle(int event) -> int override; ///< Handle the events, in particular 'q' and 'x'.

    protected:
        cairo_surface_t *surface = nullptr; ///< Cairo version of the surface.
        cairo_t *        context = nullptr; ///< A context to draw onto the surface.
        size_t           stride  = 0;       ///< The number of pixels in a line in memory.
        void             draw() override;   ///< Draw the contents of the window (called by FLTK).

    private:
        std::string           snapshot_prefix; ///< The filename prefix for PNG snapshots.
        int                   snapshot_number; ///< The number of the current snapshot.
        double                snapshot_period; ///< The time interval between automatic snapshots, in seconds.
        std::optional<size_t> snapshot_task;

        virtual void paint(){}; /// Update the contents of surface from a derived class data.
    };
} // namespace vb
