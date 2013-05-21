#include <vb/PRNG.h>
#include <vb/CL_Parser.h>
#include <vb/Clock.h>

using namespace vb;
using namespace std;

int main (int argc, char ** argv) {
    CL_Parser CLP (argc,argv,"n=100000000");
    int n=CLP('n');
    long long s;
    double t=0;

    string state = prng.state();

    cerr << "vb::PRNG::bernoulli() ...              \ttime = ";
    s=0; t=global_clock.time();
    for (int i=0; i<n; ++i) s += prng.bernoulli();
    cerr << global_clock.time()-t << ",  \tsum = " << s << endl;

    cerr << "direct vb::PRNG() ...                  \ttime = ";
    s=0; t=global_clock.time();
    for (int i=0; i<n; ++i) s += prng()%2;
    cerr << global_clock.time()-t << ",  \tsum = " << s << endl;

    cerr << "boost::bernoulli_distribution ...      \ttime = ";
    s=0; t=global_clock.time();
    boost::bernoulli_distribution<> dist(.5);
    for (int i=0; i<n; ++i) s += dist(prng);
    cerr << global_clock.time()-t << ",  \tsum = " << s << endl;

    cerr << "many boost::bernoulli_distributions ...\ttime = ";
    s=0; t=global_clock.time();
    for (int i=0; i<n; ++i) s += (boost::bernoulli_distribution<> (.5))(prng);
    cerr << global_clock.time()-t << ",  \tsum = " << s << endl;

    cerr << "UNIX rand() ...                        \ttime = ";
    s=0; t=global_clock.time();
    for (int i=0; i<n; ++i) s += rand()%2;
    cerr << global_clock.time()-t << ",  \tsum = " << s << endl;

    cerr << "Stored state, first version ...        \ttime = ";
    prng.state(state); s=0; t=global_clock.time();
    for (int i=0; i<n; ++i) s += prng.bernoulli();
    cerr << global_clock.time()-t << ",  \tsum = " << s << endl;
}
