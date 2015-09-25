
/// @file sample.cpp
/// A simple example of how to use the vb::Image class.

#include <vb/Hub.h>
#include <vb/Image.h>

using namespace vb;

class Sample : public Image {
public:
	int n;

	Sample (int _n) : Image (_n,_n), n(_n) {}

	vb::Color compute (coo z) {
		int d = norm (z-coo(n/2,n/2));
		return vb::Color ( 1+(2*d*((1<<8)-1))/(n*n) );
	}
};

int main (int argc, char *argv[]) {
	H.init ("Sample",argc,argv,"n=500,v");
	int n = H['n'];
	Sample S (n);
	if (H['v']) S.show();
	S.tessel ({0,0}, {n-1,n-1}, [&](coo c){ return S.compute(c); });
	S.output("sample");
	return 0;
}
