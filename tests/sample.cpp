#include <vb/Coloring.h>
#include <vb/Hub.h>

using namespace vb;

Color wave (cpx zz) { return Color (int(256*norm(zz))%256); }

int main (int argc, char *argv[]) {
	H.init ("Sample",argc,argv,"n=500,a");
	Coloring S (cpx(-1,0),cpx(1,1.2),H['n'],wave); S.aa = H['a'];
	S.show(); S.pause(); S.output();
}
