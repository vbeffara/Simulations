#include <vb/Cluster.h>

namespace vb {
    bool Cluster::fits(coo z) const { return (z.x >= ul.x) && (z.y >= ul.y) && (z.x < ul.x + int(w)) && (z.y < ul.y + int(w)); }

    void Cluster::ensure_sub() {
        if (!sub.empty()) return;
        int64_t ww = w / 3;
        for (int64_t y = 0; y < 3; ++y)
            for (int64_t x = 0; x < 3; ++x) sub.emplace_back(ul + coo{x * ww, y * ww}, ww);
        if (np == w * w)
            for (auto &c : sub) c.np = size_t(ww * ww);
    }

    void Cluster::grow() {
        if (np != 0) {
            std::vector<Cluster> bak;
            bak.swap(sub);
            ul.x -= w;
            ul.y -= w;
            w *= 3;
            ensure_sub();
            Cluster &c = sub[4];
            tile.swap(c.tile);
            c.np = np;
            c.sub.swap(bak);
        } else {
            ul.x -= w;
            ul.y -= w;
            w *= 3;
        }
    }

    bool Cluster::at(coo z) const {
        if ((np == 0) || (!fits(z))) return false;
        if (np == w * w) return true;
        if (!sub.empty()) return sub[sub_index(z)].at(z);
        int64_t x = z.x - ul.x, y = z.y - ul.y;
        auto    xy = size_t(x + int(w) * y);
        return tile[xy];
    }

    void Cluster::put(coo z, bool b) {
        size_t plain = b ? w * w : 0;
        if (np == plain) return;
        if (w == bs) {
            if (tile.empty()) tile.resize(w * w, !b);
            int64_t x = z.x - ul.x, y = z.y - ul.y, xy = x + int(w) * y;
            if (b)
                ++np;
            else
                --np;
            tile[size_t(xy)] = b;
            if (np == plain) std::vector<bool>().swap(tile);
        } else {
            ensure_sub();
            if (b)
                ++np;
            else
                --np;
            sub[sub_index(z)].put(z, b);
            if (np == plain) std::vector<Cluster>().swap(sub);
        }
    }

    void Cluster::insert(coo z) {
        while (!fits(z)) grow();
        if (!at(z)) put(z, true);
    }

    void Cluster::remove(coo z) {
        if (at(z)) put(z, false);
    }

    void Cluster::validate() {
        if ((np == 0) || (np == w * w)) {
            assert(!tile.size());
            assert(!sub.size());
        }
        if (!sub.empty()) {
            int64_t s = 0;
            for (auto &c : sub) {
                c.validate();
                s += c.np;
            }
            assert(s == np);
        }
    }

    void Cluster::paint(Image &I, coo ul, coo br) {
        if (ul == br) {
            ul = {0, 0};
            br = {I.w(), I.h()};
        }
        if (br.x - ul.x > int64_t(w)) {
            ul = (ul + br - coo{int64_t(w), int64_t(w)}) / 2;
            br = ul + coo{int64_t(w), int64_t(w)};
        }
        if (np == 0) {
            for (size_t x = ul.x; x < br.x; ++x)
                for (size_t y = ul.y; y < br.y; ++y) I.put({x, y}, BLACK);
        } else if (np == w * w) {
            for (size_t x = ul.x; x < br.x; ++x)
                for (size_t y = ul.y; y < br.y; ++y)
                    I.put({x, y}, (x == ul.x) || (x == br.x - 1) || (y == ul.y) || (y == br.y - 1) ? RED : BLUE);
        } else if (br == ul + coo{1, 1}) {
            I.put(ul, Grey(uint8_t((255 * np) / (w * w))));
        } else if (!sub.empty()) {
            int64_t ww = br.x - ul.x, hh = br.y - ul.y;
            for (int64_t x = 0; x < 3; ++x)
                for (int64_t y = 0; y < 3; ++y)
                    sub[size_t(x + 3 * y)].paint(I, ul + coo{x * ww / 3, y * hh / 3}, ul + coo{(x + 1) * ww / 3, (y + 1) * hh / 3});
        } else {
            int64_t ww = br.x - ul.x, s = int(w) / ww;
            for (int64_t x = 0; x < ww; ++x)
                for (int64_t y = 0; y < ww; ++y) I.put(ul + coo{x, y}, tile[size_t(x * s + int(w) * y * s)] ? WHITE : BLACK);
        }
    }

    size_t Cluster::sub_index(coo z) const {
        int64_t ww = w / 3, i = (z.x - ul.x) / ww, j = (z.y - ul.y) / ww;
        return size_t(i + 3 * j);
    }

    size_t Cluster::bs = 16;
} // namespace vb
