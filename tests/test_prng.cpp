#include <vb/Hub.h>
#include <vb/PRNG.h>
#include <random>
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_opadd.h>

using namespace vb;
using namespace std;

double time () { return double(clock()) / CLOCKS_PER_SEC; }

int main (int argc, char ** argv) {
	H.init ("Testing PRNG methods", argc,argv, "n=100000000");
	int n=H['n'];
	double s, t=0;

	string state = prng.state();

	cerr << "vb::PRNG::gaussian() ...               \ttime = ";
	s=0; t=time();
	for (int i=0; i<n; ++i) { double o = prng.gaussian(); s += o*o; }
	cerr << time()-t << ",  \tsum = " << s << endl;

	cerr << "boost::gaussian_distribution ...       \ttime = ";
	s=0; t=time();
	boost::normal_distribution<> dist(0,1);
	boost::mt19937 boostengine;
	for (int i=0; i<n; ++i) { double o = dist(boostengine); s += o*o; }
	cerr << time()-t << ",  \tsum = " << s << endl;

	cerr << "many boost::gaussian_distributions ...\ttime = ";
	s=0; t=time();
	for (int i=0; i<n; ++i) { double o = (boost::normal_distribution<> (0,1))(prng); s += o*o; }
	cerr << time()-t << ",  \tsum = " << s << endl;

	cerr << "C++11 style ...                        \ttime = ";
	s=0; t=time();
	std::normal_distribution<> dist2(0,1);
	std::mt19937_64 engine;
	for (int i=0; i<n; ++i) { double o = dist2(engine); s += o*o; }
	cerr << time()-t << ",  \tsum = " << s << endl;

	cerr << "Stored state, first version ...        \ttime = ";
	prng.state(state); s=0; t=time();
	for (int i=0; i<n; ++i) { double o = prng.gaussian(); s += o*o; }
	cerr << time()-t << ",  \tsum = " << s << endl;

	cerr << "CILK based multithreaded               \ttime = ";
	auto go = []() { static thread_local PRNG p; auto o = p.gaussian(); return o*o; };
	cilk::reducer <cilk::op_add<double>> ps (0);
	cilk_for (int i=0; i<n; ++i) *ps += go();
	cerr << time()-t << ",  \tsum = " << ps.get_value() << endl;
}
