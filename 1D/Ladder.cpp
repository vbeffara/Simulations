
/*
 * env[i] = nb of unvisited edges on top of [i,i+1]
 *        = 2 to the right of max(range)==env.size()
 */

#include <vb/util/Hub.h>
#include <vb/util/PRNG.h>

using std::cout;
using std::endl;

int main(int argc, char **argv) {
    vb::Hub H("Ladder", argc, argv, "t=20,d");
    int     t = H['t'];
    bool    d = H['d'];

    std::vector<int> env;
    int              X = 0;

    for (int i = 0; i < t; ++i) {
        if (d) {
            for (unsigned j = 0; j < env.size(); ++j) {
                if (X == int(j))
                    cout << "*";
                else
                    cout << " ";
                cout << env[j];
            }
            if (X == int(env.size()))
                cout << "*";
            else
                cout << " ";
            cout << "2 2 2 ..." << endl;
        } else
            cout << X << endl;

        double p;
        if (X == 0)
            p = 1;
        else
            p = (env[X - 1] == 1 ? 2.0 / 3.0 : 1.0 / 2.0);

        int dX = vb::prng.bernoulli(p) ? 1 : -1;

        if ((X == int(env.size())) && (dX == 1)) env.push_back(2);
        if ((dX == 1) && (env[X] == 1) && vb::prng.bernoulli(.5)) env[X] = 0;
        if ((dX == 1) && (env[X] == 2)) env[X] = 1;
        X += dX;
    }
}
