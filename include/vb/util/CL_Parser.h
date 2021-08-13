#pragma once /// \file
#include <map>
#include <string>

// TODO remove all of this

namespace vb {
    class Value : public std::string {
    public:
        explicit Value(std::string s = "") : std::string(std::move(s)) {}
        auto operator=(const std::string &s) -> Value & {
            std::string::operator=(s);
            return *this;
        }

        operator bool() const { return bool(strtol(c_str(), nullptr, 10)); }                                          // NOLINT
        operator int() const { return int(strtol(c_str(), nullptr, 10)); }                                            // NOLINT
        operator long() const { return long(strtol(c_str(), nullptr, 10)); }                                          // NOLINT
        operator long long() const { return static_cast<long long>(strtol(c_str(), nullptr, 10)); }                   // NOLINT
        operator unsigned() const { return unsigned(strtol(c_str(), nullptr, 10)); }                                  // NOLINT
        operator unsigned long() const { return static_cast<unsigned long>(strtol(c_str(), nullptr, 10)); }           // NOLINT
        operator unsigned long long() const { return static_cast<unsigned long long>(strtol(c_str(), nullptr, 10)); } // NOLINT
        operator double() const { return strtod(c_str(), nullptr); }                                                  // NOLINT
    };

    class CL_Parser : public std::map<char, Value> {
    public:
        CL_Parser(std::string t, int argc, char **argv, std::string c = "");

        auto operator[](char c) const -> const Value & { return find(c)->second; }

        std::string title, help, prog, dir, cmd;
    };
} // namespace vb
