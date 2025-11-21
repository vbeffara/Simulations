#pragma once
#include <spdlog/spdlog.h>

namespace vb {
    class CLP {
    public:
        CLP(int argc, char **argv, std::string desc);
        ~CLP();

        auto flag(const std::string &c, const std::string &d) -> bool;

        auto param(const std::string &c, double t, const std::string &d) -> double;
        auto param(const std::string &c, int t, const std::string &d) -> int;
        auto param(const std::string &c, size_t t, const std::string &d) -> size_t;
        auto param(const std::string &c, std::string t, const std::string &d) -> std::string;

        void finalize(); // TODO: find better name

        std::string desc, title;

    private:
        auto get_param(const std::string &c, const std::string &type, std::string val, const std::string &dsc) -> std::string;

        std::vector<std::string> args, help, flags;
        bool                     finalized = false;
    };
} // namespace vb
