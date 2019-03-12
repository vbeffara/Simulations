#pragma once /// \file
#include <map>
#include <string>

namespace vb {
    class Value : public std::string {
    public:
        explicit Value(std::string s = "") : std::string(std::move(s)) {}
        Value &operator=(const std::string &s) {
            std::string::operator=(s);
            return *this;
        }

        operator bool() const { return bool(strtol(c_str(), (char **)nullptr, 10)); } // NOLINT implicit conversion is the point

        operator int() const { return int(strtol(c_str(), (char **)nullptr, 10)); }               // NOLINT implicit conversion is the point
        operator long() const { return long(strtol(c_str(), (char **)nullptr, 10)); }             // NOLINT implicit conversion is the point
        operator long long() const { return (long long)(strtol(c_str(), (char **)nullptr, 10)); } // NOLINT implicit conversion is the point

        operator unsigned() const { return unsigned(strtol(c_str(), (char **)nullptr, 10)); } // NOLINT implicit conversion is the point
        operator unsigned long() const {
            return (unsigned long)(strtol(c_str(), (char **)nullptr, 10));
        } // NOLINT implicit conversion is the point
        operator unsigned long long() const {
            return (unsigned long long)(strtol(c_str(), (char **)nullptr, 10));
        } // NOLINT implicit conversion is the point

        operator double() const { return strtod(c_str(), (char **)nullptr); } // NOLINT implicit conversion is the point
    };

    class CL_Parser : public std::map<char, Value> {
    public:
        CL_Parser(std::string t, int argc, char **argv, std::string c = "");

        const Value &operator[](char c) const { return find(c)->second; }

        std::string title, help, prog, dir, cmd;
    };

#ifdef UNIT_TESTS
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
        char *    argv[] = {(char *)"test_cl_parser", (char *)"-s", (char *)"3", (char *)"-u"};
        CL_Parser CLP("Title", 4, argv, "s=5,t=7,u,v");
        CHECK(int(CLP['t']) == 7);
        CHECK(int(CLP['s']) == 3);
        CHECK(CLP['u']);
        CHECK(!CLP['v']);
    }
#endif
} // namespace vb
