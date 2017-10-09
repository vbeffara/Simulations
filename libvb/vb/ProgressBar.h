#pragma once /// \file

#include <vb/Auto.h>

namespace vb {
    class ProgressBar : public Auto {
    public:
        explicit ProgressBar (int length=100, double pow=1.0);
        ProgressBar (const ProgressBar &) = default;
        ProgressBar (ProgressBar &&) = default;

        ~ProgressBar () override;

        ProgressBar & operator= (const ProgressBar &) = default;
        ProgressBar & operator= (ProgressBar &&) = default;

        void set (int pos);

        void update() override;

    private:
        int final, current=0;
        double power;
    };
} // namespace vb
