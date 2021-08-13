#pragma once /// \file

#include <vb/Auto.h>

namespace vb {
    class ProgressBar : public Auto {
    public:
        explicit ProgressBar(size_t length = 100, double pow = 1.0);

        void set(size_t pos);

        void update() override;

    private:
        size_t final, current = 0;
        double power;
    };
} // namespace vb
