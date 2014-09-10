#include <vb/PRNG.h>
#include <vb/Image.h>

using namespace vb;

int main (int, char **) {
    Image I (500,500,"Testing C++11 features");

    // Loop over the whole image
    for (auto & v : I) v = prng.uniform_int(255);

    // More explicit use of iterator
    for (auto z = I.begin(); z != Array_iterator<Color> (I,coo(0,400)); ++z) *z = prng.uniform_int(128);

    // Can also use coordinates
    for (auto z = I.begin(); z != Image::iterator (I,coo(0,300)); ++z) *z = prng.uniform_int(1+255*z.x/499);

    // Even more syntactic sugar
    for (auto z = I.begin(); z != decltype(z) (I,coo(0,200)); ++z) *z = prng.uniform_int(1+255*(499-z.x)/499);

    // And even more (with a comparison shortcut)
    for (auto z = I.begin(); z != coo(0,100); ++z) *z = prng.uniform_int(1+255*z.y/99);

    I.show(); I.pause();
}
