#include <vb/Hub.h>
#include <vb/cpx.h>
#include <chrono>

using namespace vb; using namespace std; using namespace cln;

double time () {
	static std::chrono::system_clock C;
	static auto basetime = C.now();
	chrono::duration<double> dur = C.now() - basetime;
	return dur.count();
}

template <typename T> void test (string s, int n) {
	double t = time(); T z (0);
	for (int i=0; i<n; ++i) z = exp(-z);
	ostringstream os; os << time()-t;
	cout << s << " | time = " << os.str() << "\t | result = " << z << endl;
}

template <typename T> void test_ (string s, int n, const char * in) {
	double t = time(); cl_F z = in;
	for (int i=0; i<n; ++i) z = exp(-z);
	ostringstream os; os << time()-t;
	cout << s << " | time = " << os.str() << "\t | result = " << z << endl;
}

int main (int argc, char ** argv) {
	H.init ("Testing numerical types", argc,argv, "n=100000");
	int n=H['n'];

	test <float>       ("Standard float              ", n);
	test <double>      ("Standard double             ", n);
	test <long double> ("Long double                 ", n);
	test_<cl_SF>       ("CLN short float             ", n, "0s0");
	test_<cl_FF>       ("CLN single float            ", n, "0f0");
	test_<cl_DF>       ("CLN double float            ", n, "0d0");
	test <real_t>      ("CLN long float (100 digits) ", n);
	default_float_format = float_format(200);
	test <real_t>      ("CLN long float (200 digits) ", n);
}
