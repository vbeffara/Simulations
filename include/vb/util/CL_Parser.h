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

        operator bool() const { return strtol(c_str(), (char **)nullptr, 10); }     // NOLINT implicit conversion is the point
        operator int() const { return strtol(c_str(), (char **)nullptr, 10); }      // NOLINT implicit conversion is the point
        operator unsigned() const { return strtol(c_str(), (char **)nullptr, 10); } // NOLINT implicit conversion is the point
        operator int64_t() const { return strtol(c_str(), (char **)nullptr, 10); }  // NOLINT implicit conversion is the point
        operator double() const { return strtod(c_str(), (char **)nullptr); }       // NOLINT implicit conversion is the point
    };

    class CL_Parser : public std::map<char, Value> {
    public:
        void init(std::string t, int argc, char **argv, std::string c = "");

        const Value &operator[](char c) const { return find(c)->second; }

        std::string title = "undefined", help = "undefined", prog = "undefined", dir = "output/", cmd = "undefined";
    };
} // namespace vb
