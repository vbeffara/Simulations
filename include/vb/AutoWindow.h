#pragma once /// \file
#include <FL/Fl.H>
#include <future>
#include <vb/Auto.h>
#include <vb/util/coo.h>

namespace vb {
    static inline void close_window(Fl_Widget *) { exit(1); }

    template <typename T> class AutoWindow : public Auto, public T {
    public:
        AutoWindow(const std::string &s, ucoo size) : Auto(.1), T(int(size.x), int(size.y)), title(s) {
            T::label(title.c_str());
            T::callback(close_window);
        }

        ucoo pixel_size() { return {static_cast<size_t>(T::pixel_w()), static_cast<size_t>(T::pixel_h())}; }

        void update() override {
            if (T::visible()) {
                T::redraw();
                Fl::check();
                while (paused) Fl::wait();
            }
        }

        void show() override {
            Fl::use_high_res_GL(1);
            T::show();
            update();
            start = now();
        }

        bool visible() { return static_cast<bool>(T::visible()); }

        void pause() {
            paused = true;
            update();
        }

        void run(std::function<void()> f) {
            hold    = true;
            auto ff = std::async(std::launch::async, f);
            while (ff.wait_for(std::chrono::milliseconds(100)) != std::future_status::ready) update();
            ff.get();
            hold = false;
            if (die) exit(0);
        }

        int handle(int event) override {
            if (event == FL_KEYDOWN) switch (Fl::event_key()) {
                case 'q':
                    die = true;
                    return 1;
                    break;
                case 'h':
                    T::hide();
                    return 1;
                    break;
                case ' ':
                    paused = !paused;
                    return 1;
                    break;
                }
            return T::handle(event);
        }

    private:
        bool        paused = false;
        std::string title;
    };
} // namespace vb
