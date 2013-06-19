#include <vb/PRNG.h>
#include <vb/Image.h>

using namespace vb;

int main (int argc, char ** argv) {
    Image I (500,500,"Testing C++11 features");
    for (auto & v : I) v = prng.uniform_int(255);
    I.show(); I.pause();
}
