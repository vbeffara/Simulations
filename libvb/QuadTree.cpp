#include <vb/QuadTree.h>

namespace vb {
    std::vector<std::unique_ptr<QuadTree>> QuadTree::store;

    QuadTree::QuadTree(coo UL, coo BR, size_t M) : n(0), ul(UL), br(BR), center((ul + br) / 2), iul(br), ibr(ul), m(M), ch(0) {}

    auto QuadTree::index(coo z) const -> size_t {
        if (z.y < center.y) return (z.x < center.x) ? 0 : 1;
        return (z.x < center.x) ? 2 : 3;
    }

    void QuadTree::insert(coo z) {
        iul.x = std::min(iul.x, z.x);
        iul.y = std::min(iul.y, z.y);
        ibr.x = std::max(ibr.x, z.x);
        ibr.y = std::max(ibr.y, z.y);
        if (n == m) split();
        if (n < m)
            pts.push_back(z);
        else
            store[ch + index(z)]->insert(z);
        ++n;
    }

    void QuadTree::split() {
        ch = store.size();
        store.push_back(std::make_unique<QuadTree>(ul, center, m));
        store.push_back(std::make_unique<QuadTree>(coo{center.x, ul.y}, coo{br.x, center.y}, m));
        store.push_back(std::make_unique<QuadTree>(coo{ul.x, center.y}, coo{center.x, br.y}, m));
        store.push_back(std::make_unique<QuadTree>(center, br, m));
        for (auto &z : pts) store[ch + index(z)]->insert(z);
        std::vector<coo>().swap(pts);
    }

    auto QuadTree::idist(coo z) const -> int64_t { return std::min({z.x - ul.x, z.y - ul.y, br.x - z.x, br.y - z.y}); }
    auto QuadTree::odist(coo z) const -> int64_t { return std::max({z.x - ibr.x, z.y - ibr.y, iul.x - z.x, iul.y - z.y}); }

    void QuadTree::nn(coo z, QuadIndex &qi) const {
        if (n <= m)
            for (const auto &w : pts) {
                auto newnorm = sup(z - w);
                if (newnorm < qi.d) {
                    qi.d = newnorm;
                    qi.z = w;
                }
            }
        else {
            auto i0 = index(z);
            if (store[ch + i0]->n != 0) store[ch + i0]->nn(z, qi);
            for (unsigned i = 0; i < 4; ++i)
                if (i != i0) {
                    QuadTree *q = store[ch + i].get();
                    if (((q->n) != 0) && (q->odist(z) < qi.d)) q->nn(z, qi);
                }
        }
    }

    void QuadTree::paint(Image &img, ucoo u_l, size_t w) const {
        if (w == 1) {
            img.at(u_l) = (n > 0) ? GREEN : BLACK;
            return;
        }
        if (n > m) {
            store[ch + 0]->paint(img, u_l, w / 2);
            store[ch + 1]->paint(img, u_l + ucoo{w / 2, 0}, w / 2);
            store[ch + 2]->paint(img, u_l + ucoo{0, w / 2}, w / 2);
            store[ch + 3]->paint(img, u_l + ucoo{w / 2, w / 2}, w / 2);
            return;
        }
        for (size_t i = 0; i < w; ++i) {
            img.at(u_l + ucoo{i, 0}) = Color(128, 0, 0);
            img.at(u_l + ucoo{0, i}) = Color(128, 0, 0);
        }
    }
} // namespace vb
