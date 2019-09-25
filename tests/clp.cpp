#include <vb/util/CLP.h>

using namespace vb;

auto main(int argc, char **argv) -> int {
    CLP  clp(argc, argv, "This is a test of the new command line parser");
    auto r = clp.param("r", 1.0, "Rate of dummyfication");
    auto n = clp.param("n", 123, "Number of dummyfications");
    auto s = clp.flag("s", "Use alternative algorithm");
    clp.finalize();
    spdlog::info("Final state: s = {}, r = {}, n = {}", s, r, n);
}
