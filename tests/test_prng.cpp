#include <vb/PRNG.h>
#include <vb/CL_Parser.h>
#include <vb/Clock.h>

using namespace vb;
using namespace std;

int main (int argc, char ** argv) {
    CL_Parser CLP (argc,argv,"n=500000000");
    int n=CLP('n');
    long long s;
    double t=0;

    cerr << "vb::PRNG::bernoulli(.5) ...       ";
    s=0; t=global_clock.time();
    for (int i=0; i<n; ++i) s += prng.bernoulli(.5);
    cerr << global_clock.time()-t << ", sum = " << s << endl;

    cerr << "vb::PRNG::bernoulli() ...         ";
    s=0; t=global_clock.time();
    for (int i=0; i<n; ++i) s += prng.bernoulli();
    cerr << global_clock.time()-t << ", sum = " << s << endl;

    cerr << "direct vb::PRNG()%2 ...           ";
    s=0; t=global_clock.time();
    for (int i=0; i<n; ++i) s += prng()%2;
    cerr << global_clock.time()-t << ", sum = " << s << endl;

    cerr << "boost::bernoulli_distribution ... ";
    s=0; t=global_clock.time();
    boost::bernoulli_distribution<> dist(.5);
    for (int i=0; i<n; ++i) s += dist(prng);
    cerr << global_clock.time()-t << ", sum = " << s << endl;
}
