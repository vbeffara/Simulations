#include <spdlog/spdlog.h>
#include <vb/ProgressBar.h>

namespace vb {
    ProgressBar::ProgressBar(int length, double pow) : Auto(1), final(length), power(pow) {}

    void ProgressBar::set(int pos) {
        if (pos < 0) pos = 0;
        if (pos > final) pos = final;
        current = pos;
        step();
    }

    void ProgressBar::update() {
        static const std::vector<std::string> symbols{u8" ", u8"▏", u8"▎", u8"▍", u8"▌", u8"▋", u8"▊", u8"▉", u8"█"};
        std::string                           bar, days, rest;

        int n = current * 400 / final;
        for (int i = 0; i < n / 8; ++i) bar += symbols[8];
        if (n < 400) bar += symbols[n % 8] + std::string(49 - n / 8, ' ');

        if (current > 0) {
            auto eta = int(Duration((now() - start) * (pow(double(final) / current, power) - 1)).count());
            if (eta >= 3600 * 24) days = fmt::format("{}d ", eta / 3600 / 24);
            rest = fmt::format("{}:{:02}:{:02}       ", (eta / 3600) % 24, (eta / 60) % 60, eta % 60);
        }

        spdlog::info("[{}] {}{}", bar, days, rest);
    }
} // namespace vb
