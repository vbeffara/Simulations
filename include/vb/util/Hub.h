#pragma once /// \file
#undef False
#include <boost/chrono.hpp>
#include <cstdlib>
#undef CHAR_WIDTH
#include <fmt/ostream.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <vb/util/CL_Parser.h>

namespace vb {
    template <typename T> std::string pretty(const T &t) { return fmt::format("{}", t); }

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
        ~Hub();

        void init(std::string t, int argc, char **argv, std::string c = "");

        void output_str(const std::string &l, const std::string &ls, const std::string &s, bool out = true);

        template <typename T> void output(std::string l, std::string ls, const T &x, bool out = true) { output_str(l, ls, pretty(x), out); }

        std::shared_ptr<spdlog::logger> L;

        boost::chrono::time_point<boost::chrono::process_real_cpu_clock, Duration>   start;
        boost::chrono::time_point<boost::chrono::process_user_cpu_clock, Duration>   start_u;
        boost::chrono::time_point<boost::chrono::process_system_cpu_clock, Duration> start_s;
        bool                                                                         initialized = false;

        std::vector<Output> outputs;
        unsigned            max_label_width = 0;
        std::string         version         = "undefined";
    };

    extern Hub H;

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

    TEST_CASE("vb::Hub") {
        std::vector<std::string> argv_{"cmd", "-s", "3", "-u"};
        std::vector<char *>      argv;
        argv.reserve(argv_.size());
        for (auto &s : argv_) argv.push_back(&s[0]);

        H.init("Title", 4, argv.data(), "s=5,t=7,u,v");
        CHECK(int(H['t']) == 7);
        CHECK(int(H['s']) == 3);
        CHECK(H['u']);
        CHECK(!H['v']);
    }
#endif
} // namespace vb
