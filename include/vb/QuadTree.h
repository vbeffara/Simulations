#pragma once
#include <vb/Bitmap.h>

namespace vb {
    struct QuadIndex {
        coo     z{};
        int64_t d = 0;
    };

    class QuadTree {
    public:
        QuadTree(coo UL, coo BR, size_t M);

        void insert(coo z);

        void nn(coo z, QuadIndex &qi) const;

        void paint(Image &img, ucoo u_l, size_t w) const;

        size_t n;

    private:
        static std::vector<std::unique_ptr<QuadTree>> store;

        [[nodiscard]] auto index(coo z) const -> size_t;
        [[nodiscard]] auto idist(coo z) const -> int64_t;
        [[nodiscard]] auto odist(coo z) const -> int64_t;

        void split();

        coo              ul, br, center, iul, ibr;
        size_t           m, ch;
        std::vector<coo> pts;
    };
} // namespace vb
