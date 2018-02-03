#include <boost/phoenix.hpp>
#include <iostream>

using namespace boost::phoenix;
using namespace boost::phoenix::placeholders;

auto l1 = [] (auto i) { return i; };

int main(int argc, char ** argv) {
    std::cout << val(3)() << std::endl;
    std::cout << (_1)(3) << std::endl;
    std::cout << (l1)(3) << std::endl;
}