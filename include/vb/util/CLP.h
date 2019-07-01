#pragma once

#include <spdlog/spdlog.h>

namespace vb {
    namespace {
        template <typename T> std::string type_name() = delete;
        template <> std::string           type_name<double>() { return "double"; }
        template <> std::string           type_name<int>() { return "int"; }

        template <typename T> T from_string(const std::string &) = delete;
        template <> double      from_string<double>(const std::string &s) { return strtod(s.c_str(), (char **)nullptr); }
        template <> int         from_string<int>(const std::string &s) { return int(strtol(s.c_str(), (char **)nullptr, 10)); }
    } // namespace

    class CLP {
    public:
        CLP(int argc, char **argv, std::string desc);

        bool                    operator()(std::string c, std::string d);
        template <typename T> T operator()(std::string c, T t, std::string d);

        void finalize() const;

    private:
        std::vector<std::string> args, help, flags;
        std::string              desc;
    };

    template <typename T> T CLP::operator()(std::string c, T t, std::string d) {
        help.push_back(fmt::format("|  {}  | {:6} | {:>10} |  {}", c, type_name<T>(), t, d));
        if (auto i = std::find(begin(args), end(args), "-" + c); i != end(args)) {
            if (i + 1 == end(args)) {
                spdlog::error("Missing value for argument {}, exiting.", c);
                exit(1);
            }
            t = from_string<T>(*(i + 1));
        }
        return t;
    }
} // namespace vb
