#include <gsl/gsl>
#include <vb/util/CLP.h>

namespace vb {
    CLP::CLP(int argc, char **argv, std::string desc) : desc(move(desc)), title(this->desc + " |") {
        for (const auto &a : gsl::span(argv, argc)) args.emplace_back(a);
    }

    CLP::~CLP() {
        if (!finalized) {
            spdlog::warn("CLP was not finalized, doing it on destruction");
            finalize();
        }
    }

    auto CLP::get_param(const std::string &c, const std::string &type, std::string val, const std::string &desc) -> std::string {
        help.push_back(fmt::format("|  {}  | {:8} | {:>10} |  {}", c, type, val, desc));
        if (auto i = std::find(begin(args), end(args), "-" + c); i != end(args)) {
            if (i + 1 == end(args)) {
                spdlog::error("Missing value for argument {}, exiting.", c);
                exit(1);
            }
            val = *(i + 1);
            args.erase(i, i + 2);
        }
        title += fmt::format(" {}={}", c, val);
        return val;
    }

    auto CLP::param(const std::string &c, int t, const std::string &d) -> int {
        auto v = get_param(c, "int", fmt::format("{}", t), d);
        return int(strtol(v.c_str(), nullptr, 10));
    }

    auto CLP::param(const std::string &c, size_t t, const std::string &d) -> size_t {
        auto v = get_param(c, "size_t", fmt::format("{}", t), d);
        return size_t(strtol(v.c_str(), nullptr, 10));
    }

    auto CLP::flag(const std::string &c, const std::string &d) -> bool {
        flags.push_back(fmt::format("                       |   {}  |  {}", c, d));
        auto i = std::find(begin(args), end(args), "-" + c);
        if (i == end(args)) return false;
        title += " " + c;
        args.erase(i);
        return true;
    }

    auto CLP::param(const std::string &c, double t, const std::string &d) -> double {
        auto v = get_param(c, "double", fmt::format("{}", t), d);
        return strtod(v.c_str(), nullptr);
    }

    void CLP::finalize() {
        finalized = true;

        if (std::find(begin(args), end(args), "-h") != end(args)) {
            spdlog::info("");
            spdlog::info("{}", desc);

            if (!flags.empty()) {
                spdlog::info("");
                spdlog::info("                       +------+");
                spdlog::info("                       | flag |");
                spdlog::info("                       +------+");
                for (const auto &s : flags) spdlog::info(s);
                spdlog::info("                       +------+");
            }
            if (!help.empty()) {
                spdlog::info("");
                spdlog::info("+-----+----------+------------+");
                spdlog::info("| arg | type     |    default |");
                spdlog::info("+-----+----------+------------+");
                for (const auto &s : help) spdlog::info(s);
                spdlog::info("+-----+----------+------------+");
            }
            spdlog::info("");
            spdlog::info("File title: {}", title);
            spdlog::info("");
            exit(0);
        }

        auto rem = args;
        rem.erase(begin(rem));
        if (!empty(rem)) spdlog::warn("Unused command line arguments: {}", fmt::join(rem, " "));
    }
} // namespace vb
