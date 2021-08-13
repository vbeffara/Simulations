#include <spdlog/spdlog.h>
#include <vb/ProgressBar.h>

namespace vb {
    ProgressBar::ProgressBar(size_t length, double pow) : Auto(1), final(length), power(pow) {}

    void ProgressBar::set(size_t pos) {
        if (pos > final) pos = final;
        current = pos;
        step();
    }

    void ProgressBar::update() {
        static const std::vector<std::string> symbols{" ", "▏", "▎", "▍", "▌", "▋", "▊", "▉", "█"};
        std::string                           bar, days, rest;

        auto n = current * 400 / final;
        for (size_t i = 0; i < n / 8; ++i) bar += symbols[8];
        if (n < 400) bar += symbols[n % 8] + std::string(size_t(49 - n / 8), ' ');

        if (current > 0) {
            auto eta = int(Duration((now() - start) * (pow(double(final) / double(current), power) - 1)).count());
            if (eta >= 3600 * 24) days = fmt::format("{}d ", eta / 3600 / 24);
            rest = fmt::format("{}:{:02}:{:02}       ", (eta / 3600) % 24, (eta / 60) % 60, eta % 60);
        }

        spdlog::info("[{}] {}{}", bar, days, rest);
    }
} // namespace vb
