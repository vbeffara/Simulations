#pragma once /// \file
#include <FL/Fl.H>
#include <future>
#include <vb/Auto.h>
#include <vb/util/Hub.h>

namespace vb {
    static inline void close_window(Fl_Widget *) { exit(1); }

    template <typename T> class AutoWindow : public Auto, public T {
    public:
        // TODO: remove Hub reference here, put string instead
        AutoWindow(const Hub &H, int wd, int ht) : Auto(.1), T(wd, ht), paused(false) {
            T::label(H.title.c_str());
            T::callback(close_window);
        }

        void update() override {
            if (T::visible()) {
                T::redraw();
                Fl::check();
                while (paused) Fl::wait();
            }
        }

        void show() override {
            T::show();
            update();
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
        bool paused;
    };
} // namespace vb
