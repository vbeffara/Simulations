#pragma once
#include <spdlog/spdlog.h>

namespace vb {
    class CLP {
    public:
        CLP(int argc, char **argv, std::string desc);
        ~CLP();

        bool flag(const std::string &c, const std::string &d);

        double param(const std::string &c, double t, const std::string &d);
        int    param(const std::string &c, int t, const std::string &d);
        size_t param(const std::string &c, size_t t, const std::string &d);

        void finalize(); // TODO: find better name

        std::string desc, title;

    private:
        std::string get_param(const std::string &c, const std::string &type, std::string val, const std::string &desc);

        std::vector<std::string> args, help, flags;
        bool                     finalized = false;
    };
} // namespace vb
