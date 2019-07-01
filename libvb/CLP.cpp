#include <gsl/gsl>
#include <vb/util/CLP.h>

namespace vb {
    CLP::CLP(int argc, char **argv, std::string desc) : desc(desc) {
        for (const auto &a : gsl::span(argv, argc)) args.emplace_back(a);
    }

    void CLP::finalize() const {
        if (auto i = std::find(begin(args), end(args), "-h"); i != end(args)) {
            spdlog::info("");
            spdlog::info("{}", desc);

            if (!flags.empty()) {
                spdlog::info("");
                spdlog::info("                     +------+");
                spdlog::info("                     | flag |");
                spdlog::info("                     +------+");
                for (const auto &s : flags) spdlog::info(s);
                spdlog::info("                     +------+");
            }
            if (!help.empty()) {
                spdlog::info("");
                spdlog::info("+-----+--------+------------+");
                spdlog::info("| arg | type   |    default |");
                spdlog::info("+-----+--------+------------+");
                for (const auto &s : help) spdlog::info(s);
                spdlog::info("+-----+--------+------------+");
            }
            spdlog::info("");
            exit(0);
        }
    }

    bool CLP::operator()(std::string c, std::string d) {
        flags.push_back(fmt::format("                     |   {}  |  {}", c, d));
        return std::find(begin(args), end(args), "-" + c) != end(args);
    }
} // namespace vb
