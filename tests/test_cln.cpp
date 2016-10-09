#include <vb/cpx.h>
#include <vb/util.h>
#include <boost/lexical_cast.hpp>

using namespace vb; using namespace std; using namespace cln;

template <typename T> void test (const string &s, int n, const string &in = "0") {
	timing (s, [&](){
		auto z = boost::lexical_cast<T> (in);
		for (int i=0; i<n; ++i) z = exp(-z);
		return z;
	});
}

int main (int argc, char ** argv) {
	H.init ("Testing numerical types", argc,argv, "n=100000");
	int n=H['n'];

	test <float>       ("Standard float             ", n);
	test <double>      ("Standard double            ", n);
	test <long double> ("Long double                ", n);
	test <cl_F>        ("CLN short float            ", n, "0s0");
	test <cl_F>        ("CLN single float           ", n, "0f0");
	test <cl_F>        ("CLN double float           ", n, "0d0");
	test <real_t>      ("CLN long float (100 digits)", n);
	default_float_format = float_format(200);
	test <real_t>      ("CLN long float (200 digits)", n);
}
