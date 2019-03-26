#include "catch2/catch.hpp"
#include <vb/util/CL_Parser.h>

using vb::Value, vb::CL_Parser;

TEST_CASE("vb::Value") {
    Value v1("1"), v2("3.4");
    CHECK(bool(v1));
    CHECK(int(v1) == 1);
    CHECK(int64_t(v1) == 1);
    CHECK(double(v2) == 3.4);

    v1 = "45";
    CHECK(int(v1) == 45);
}

TEST_CASE("vb::CL_Parser") {
    std::vector<char *> argv{strdup("test_cl_parser"), strdup("-s"), strdup("3"), strdup("-u"), nullptr};
    CL_Parser           CLP("Title", 4, argv.data(), "s=5,t=7,u,v");
    CHECK(int(CLP['t']) == 7);
    CHECK(int(CLP['s']) == 3);
    CHECK(CLP['u']);
    CHECK(!CLP['v']);
}
