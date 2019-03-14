#include <queue>
#include <vb/Bitmap.h>
#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using namespace vb;
using namespace std;

namespace vb {
    template <> Color to_Color(int t) {
        static const vector<Color> C = {BLACK, RED, GREEN};
        return C[t];
    }
} // namespace vb

class Site {
public:
    explicit Site(int i = 0) : state(i){};
    explicit operator Color() const {
        static const vector<Color> C = {BLACK, RED, GREEN};
        return C[state];
    }
    int state;
};

class Particle {
public:
    explicit Particle(coo xy = {0, 0}, int s = 0, bool z = false, double t = 0) : state(s), location(xy), type(z), next(t){};
    bool operator<(const Particle &o) const { return next > o.next; }

    coo jump() const {
        if (type) {
            int out = prng.uniform_int(2);
            if (state == 2) out += 2;
            return dz[out];
        }
        int out = prng.uniform_int(4);
        if (out + 2 * state == 4) out = 2 - out;
        return dz[out];
    }

    int    state;
    coo    location;
    bool   type;
    double next;
};

class Sagex : public Bitmap<int> {
public:
    Sagex(const Hub &H, size_t w, size_t h) : Bitmap<int>(H.title, {w, h}) {
        for (size_t x = 0; x < w; ++x)
            for (size_t y = 0; y < h; ++y) {
                if (prng.bernoulli(H['l'])) {
                    Particle p({int(x), int(y)}, prng.bernoulli(H['p']) ? 1 : 2, H['z'], prng.exponential());
                    put({x, y}, p.state);
                    q.push(p);
                }
            }
    }

    void go(const Hub &H) {
        show();
        if (H['v']) snapshot_setup("Sagex", 1);
        while (true) {
            Particle p = q.top();
            q.pop();
            coo nz = p.location + p.jump();
            nz.x   = (nz.x + w()) % w();
            nz.y   = (nz.y + h()) % h();
            if (at(nz) == 0) {
                swap(at(p.location), at(nz));
                step();
                p.location = nz;
            }
            p.next += prng.exponential();
            q.push(p);
        }
    }

    priority_queue<Particle> q;
};

int main(int argc, char **argv) {
    Hub H("Sagex process", argc, argv, "n=400,w=0,l=.22,p=.5,v,z");
    Sagex(H, int(H['w']) != 0 ? H['w'] : H['n'], H['n']).go(H);
}
