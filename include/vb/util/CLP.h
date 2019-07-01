#pragma once

#include <spdlog/spdlog.h>

namespace vb {
    namespace {
        template <typename T> std::string type_name() = delete;
        template <> std::string           type_name<double>() { return "double"; }
        template <> std::string           type_name<size_t>() { return "size_t"; }
        template <> std::string           type_name<int>() { return "int"; }

        template <typename T> T from_string(const std::string &) = delete;
        template <> double      from_string<double>(const std::string &s) { return strtod(s.c_str(), (char **)nullptr); }
        template <> size_t      from_string<size_t>(const std::string &s) { return size_t(strtol(s.c_str(), (char **)nullptr, 10)); }
        template <> int         from_string<int>(const std::string &s) { return int(strtol(s.c_str(), (char **)nullptr, 10)); }
    } // namespace

    class CLP {
    public:
        CLP(int argc, char **argv, std::string desc);
        ~CLP();

        bool                    operator()(std::string c, std::string d);
        template <typename T> T operator()(std::string c, T t, std::string d);

        void finalize(); // TODO: find better name

    private:
        std::vector<std::string> args, help, flags;
        std::string              desc;
        bool                     finalized = false;
    };

    template <typename T> T CLP::operator()(std::string c, T t, std::string d) {
        help.push_back(fmt::format("|  {}  | {:6} | {:>10} |  {}", c, type_name<T>(), t, d));
        if (auto i = std::find(begin(args), end(args), "-" + c); i != end(args)) {
            if (i + 1 == end(args)) {
                spdlog::error("Missing value for argument {}, exiting.", c);
                exit(1);
            }
            t = from_string<T>(*(i + 1));
            args.erase(i, i + 2);
        }
        return t;
    }
} // namespace vb
