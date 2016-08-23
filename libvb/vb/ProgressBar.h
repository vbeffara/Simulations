#pragma once /// \file

#include <vb/Auto.h>

namespace vb {
    class ProgressBar : public Auto {
    public:
        ProgressBar (int length=100, double pow=1.0);
        ~ProgressBar () override;

        void set (int pos);

        void update() override;

    private:
        int final, current;
        double power;
    };
}
