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

    #ifdef UNIT_TESTS
    TEST_CASE ("vb::ProgressBar") {
        auto l = Auto::tasks.size();
        {
            ProgressBar P (1e7);
            CHECK (Auto::tasks.size() == l+1);
            CHECK (Auto::tasks.back().active);
            for (int i=0; i<1e7; ++i) P.set(i);
        }
        CHECK (!Auto::tasks.back().active);
    }
    #endif
} // namespace vb
