#include <chrono>
#ifdef CILK
#include <cilk/cilk.h>
#include <cilk/cilk_api.h>
#include <cilk/reducer_opadd.h>
#endif
#include <vb/Hub.h>
#include <vb/PRNG.h>

using namespace vb; using namespace std;

double time () {
	static std::chrono::system_clock C;
	static auto basetime = C.now();
	chrono::duration<double> dur = C.now() - basetime;
	return dur.count();
}

int main (int argc, char ** argv) {
	H.init ("Testing PRNG methods", argc,argv, "n=100000000");
	int n=H['n'];
	double s, t=0;

	string state = prng.state();

	s=0; t=time();
	for (int i=0; i<n; ++i) { double o = prng.gaussian(); s += o*o; }
	H.L->info ("{:<40} time = {:<10} sum = {}", "vb::PRNG::gaussian", time()-t, s);

	s=0; t=time();
	boost::normal_distribution<> dist(0,1);
	boost::mt19937_64 boostengine;
	for (int i=0; i<n; ++i) { double o = dist(boostengine); s += o*o; }
	H.L->info ("{:<40} time = {:<10} sum = {}", "boost::gaussian_distribution", time()-t, s);

	s=0; t=time();
	for (int i=0; i<n; ++i) { double o = (boost::normal_distribution<> (0,1))(prng); s += o*o; }
	H.L->info ("{:<40} time = {:<10} sum = {}", "many boost::gaussian_distributions", time()-t, s);

	s=0; t=time();
	std::normal_distribution<> dist2(0,1);
	std::mt19937_64 engine;
	for (int i=0; i<n; ++i) { double o = dist2(engine); s += o*o; }
	H.L->info ("{:<40} time = {:<10} sum = {}", "C++11 style", time()-t, s);

	prng.state(state); s=0; t=time();
	for (int i=0; i<n; ++i) { double o = prng.gaussian(); s += o*o; }
	H.L->info ("{:<40} time = {:<10} sum = {}", "Stored state, first version", time()-t, s);

#ifdef CILK
	auto go = []() { static thread_local PRNG p; auto o = p.gaussian(); return o*o; };
	cilk::reducer <cilk::op_add<double>> ps (0); t=time();
	cilk_for (int i=0; i<n; ++i) *ps += go();
	H.L->info ("{:<40} time = {:<10} sum = {}", "CILK based multithreaded", time()-t, s);
#endif
}
