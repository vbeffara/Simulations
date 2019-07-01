#include <vb/util/CLP.h>

using namespace vb;

int main(int argc, char **argv) {
    CLP  clp(argc, argv, "This is a test of the new command line parser");
    auto s = clp("s", "Use alternative algorithm");
    auto r = clp("r", 1.0, "Rate of dummyfication");
    auto n = clp("n", 123, "Number of dummyfications");
    clp.finalize();
    spdlog::info("Final state: s = {}, r = {}, n = {}", s, r, n);
}
