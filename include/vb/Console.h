#pragma once /// \file
#include <FL/Fl_Hor_Nice_Slider.H>
#include <FL/Fl_Output.H>
#include <FL/Fl_Window.H>
#include <vb/AutoWindow.h>
#include <vb/util/cpx.h>

namespace vb {
    template <typename T> void runinput(Fl_Widget *W);

    class Console : public AutoWindow<Fl_Window> {
    public:
        template <typename T> struct Input : public Fl_Hor_Nice_Slider {
            Input(T t, T t1, T t2, std::function<void(T)> cb, int w, int h, Console *c)
                : Fl_Hor_Nice_Slider(0, h, w, 30), f(move(cb)), c(c) {
                bounds(t1, t2);
                value(t);
                callback(runinput<T>);
            }
            std::function<void(T)> f;
            Console *              c;
        };

        template <typename T> struct Output : public Fl_Output {
            Output(std::function<T()> f, const char *n, int w, int h) : Fl_Output(0, h, w - 150, 30, n), f(move(f)) {
                align(FL_ALIGN_RIGHT);
            }
            void draw() override {
                value(fmt::format("{}", f()).c_str());
                Fl_Output::draw();
            }
            std::function<T()> f;
        };

        // TODO: remove Hub ref here
        Console(const Hub &H) : AutoWindow(H, {500, 0}) { label("Console"); }

        void add(Fl_Widget *S) {
            size(w(), h() + S->h());
            AutoWindow::add(S);
        }

        template <typename T> void lambda(std::function<T()> f, const char *n) { add(new Output<T>(f, n, w(), h())); }
        template <typename T> void watch(T &t, const char *n) {
            lambda<T>([&t] { return t; }, n);
        }

        template <typename T> void input(T t, T t1, T t2, std::function<void(T)> f) {
            add(new Input<T>(t, t1, t2, move(f), w(), h(), this));
        }

        template <typename T> void manage(T &t, T t1, T t2, const char *n, const std::function<void()> &cb) {
            input<T>(t, t1, t2, [cb, &t](T tt) {
                t = tt;
                cb();
            });
            if (n) watch(t, n);
        }

        template <typename T> void manage(T &t, T t1, T t2, const char *n) {
            manage(t, t1, t2, n, [] {});
        }
    };

    template <> inline void Console::manage(cpx &t, cpx t1, cpx t2, const char *n, const std::function<void()> &cb) {
        input<double>(real(t), real(t1), real(t2), [cb, &t](double tt) {
            t = {tt, imag(t)};
            cb();
        });
        input<double>(imag(t), imag(t1), imag(t2), [cb, &t](double tt) {
            t = {real(t), tt};
            cb();
        });
        if (n) watch(t, n);
    }

    template <typename T> void runinput(Fl_Widget *W) {
        auto M = static_cast<Console::Input<T> *>(W);
        M->f(M->value());
        M->c->redraw();
    }
} // namespace vb
