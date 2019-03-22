#pragma once
#include <vb/Bitmap.h>

namespace vb {
    struct QuadIndex {
        coo  z;
        long d = 0;
    };

    class QuadTree {
    public:
        QuadTree(coo UL, coo BR, size_t M);

        void insert(coo z);

        void nn(coo z, QuadIndex &qi) const;

        void paint(Image &img, ucoo ul, size_t w);

        size_t n;

    private:
        static std::vector<std::unique_ptr<QuadTree>> store;

        unsigned index(coo z) const;
        int64_t  idist(coo z) const;
        int64_t  odist(coo z) const;

        void split();

        coo              ul, br, center, iul, ibr;
        size_t           m, ch;
        std::vector<coo> pts;
    };
} // namespace vb
