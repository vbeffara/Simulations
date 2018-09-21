#pragma once
#include <vb/Bitmap.h>

namespace vb {
    class Cluster {
    public:
        Cluster(coo ul_ = {-bs / 2, -bs / 2}, int64_t w_ = bs) : ul(ul_), w(w_){};

        void ensure_sub();
        void grow();

        bool fits(coo z) const;
        bool at(coo z) const;

        void put(coo z, bool b);
        void insert(coo z);
        void remove(coo z);

        void validate();
        void paint(Image & I, coo ul = {0, 0}, coo br = {0, 0});

        int64_t sub_index(coo z) const;

        coo                  ul;
        int64_t              w, np = 0;
        std::vector<bool>    tile;
        std::vector<Cluster> sub;
        static int64_t       bs;
    };

#ifdef UNIT_TESTS
    TEST_CASE("vb::Cluster") {
        Cluster C;
        C.insert({35, 42});
        C.insert({1234, 5678});
        C.insert({91823749, -2793474});
        C.remove({1234, 5678});

        CHECK(C.at({35, 42}));
        CHECK(!C.at({1234, 5678}));
        CHECK(C.at({91823749, -2793474}));
        CHECK(!C.at({3, 4}));
        CHECK(!C.at({981327, 2371827}));
    }
#endif
} // namespace vb
