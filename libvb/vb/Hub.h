#pragma once /// \file
#undef False
#include <boost/chrono.hpp>
#include <cstdlib>
#include <map>
#undef CHAR_WIDTH
#include <spdlog/fmt/ostr.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <string>

namespace vb {
    template <typename T> std::string pretty(const T & t) { return fmt::format("{}", t); }

    using Duration  = boost::chrono::duration<double>;
    using TimePoint = boost::chrono::time_point<boost::chrono::process_real_cpu_clock, Duration>;

    class Value : public std::string {
    public:
        explicit Value(std::string s = "") : std::string(std::move(s)) {}
        Value & operator=(const std::string & s) {
            std::string::operator=(s);
            return *this;
        }

        operator bool() const { return strtol(c_str(), (char **)nullptr, 10); }     // NOLINT implicit conversion is the point
        operator int() const { return strtol(c_str(), (char **)nullptr, 10); }      // NOLINT implicit conversion is the point
        operator unsigned() const { return strtol(c_str(), (char **)nullptr, 10); } // NOLINT implicit conversion is the point
        operator int64_t() const { return strtol(c_str(), (char **)nullptr, 10); }  // NOLINT implicit conversion is the point
        operator double() const { return strtod(c_str(), (char **)nullptr); }       // NOLINT implicit conversion is the point
    };

    struct Output {
        std::string label;
        std::string lshort;
        std::string value;
        bool        out;
    };

    class Hub : public std::map<char, Value> {
    public:
        Hub();
        Hub(const Hub &) = delete;
        Hub(Hub &&)      = delete;

        ~Hub();

        Hub & operator=(const Hub &) = delete;
        Hub & operator=(Hub &&) = delete;

        void init(std::string t, int argc, char ** argv, std::string c = "");

        const Value & operator[](char c) const { return find(c)->second; }

        void                       output_str(const std::string & l, const std::string & ls, const std::string & s, bool out = true);
        template <typename T> void output(std::string l, std::string ls, const T & x, bool out = true) {
            output_str(l, ls, pretty(x), out);
        }

        std::string title = "undefined", cmd = "undefined", prog = "undefined", version = "undefined", dir = "output/", help = "undefined";
        std::map<char, bool> has_arg;

        std::shared_ptr<spdlog::logger> L;

        boost::chrono::time_point<boost::chrono::process_real_cpu_clock, Duration>   start;
        boost::chrono::time_point<boost::chrono::process_user_cpu_clock, Duration>   start_u;
        boost::chrono::time_point<boost::chrono::process_system_cpu_clock, Duration> start_s;
        bool                                                                         initialized = false;

        std::vector<Output> outputs;
        int                 max_label_width = 0;
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
        for (auto & s : argv_) argv.push_back(&s[0]);

        H.init("Title", 4, argv.data(), "s=5,t=7,u,v");
        CHECK(int(H['t']) == 7);
        CHECK(int(H['s']) == 3);
        CHECK(H['u']);
        CHECK(!H['v']);
    }
#endif
} // namespace vb
