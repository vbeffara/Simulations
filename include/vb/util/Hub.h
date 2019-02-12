#pragma once /// \file
#undef False
#include <boost/chrono.hpp>
#include <cstdlib>
#undef CHAR_WIDTH
#include <fmt/ostream.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <vb/util/CL_Parser.h>

namespace vb {
    using Duration  = boost::chrono::duration<double>;
    using TimePoint = boost::chrono::time_point<boost::chrono::process_real_cpu_clock, Duration>;

    struct Output {
        std::string label;
        std::string lshort;
        std::string value;
        bool        out;
    };

    class Hub : public CL_Parser {
    public:
        Hub(std::string t, int argc, char **argv, std::string c = "");
        ~Hub();

        template <typename T> void output(std::string l, std::string ls, const T &x, bool out = true) {
            if (l.size() > max_label_width) max_label_width = l.size();
            outputs.push_back({l, ls, fmt::format("{}", x), out});
        }

        std::shared_ptr<spdlog::logger> L;

        boost::chrono::time_point<boost::chrono::process_real_cpu_clock, Duration>   start;
        boost::chrono::time_point<boost::chrono::process_user_cpu_clock, Duration>   start_u;
        boost::chrono::time_point<boost::chrono::process_system_cpu_clock, Duration> start_s;

        std::vector<Output> outputs;
        unsigned            max_label_width = 0;
        std::string         version;
    };
} // namespace vb
