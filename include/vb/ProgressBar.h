#pragma once /// \file

#include <vb/Auto.h>

namespace vb {
    class ProgressBar : public Auto {
    public:
        ProgressBar(size_t length = 100, double pow = 1.0);

        void set(size_t pos);

        void update() override;

    private:
        size_t final, current = 0;
        double power;
    };

#ifdef UNIT_TESTS
    TEST_CASE("vb::ProgressBar") {
        auto l = Auto::tasks.size();
        {
            ProgressBar P(1e7);
            CHECK(Auto::tasks.size() == l + 1);
            CHECK(Auto::tasks.back().active);
            for (size_t i = 0; i < 1e7; ++i) P.set(i);
        }
        CHECK(!Auto::tasks.back().active);
    }
#endif
} // namespace vb
