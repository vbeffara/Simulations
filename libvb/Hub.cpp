#include "gsl/gsl"
#include <vb/Hub.h>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <boost/algorithm/string.hpp>
#include <cln/cln.h>
#include <getopt.h>
#include <iostream>
#include <sys/stat.h>
#include <vector>

namespace vb {
    Hub::Hub() {
        Fl::gl_visual(FL_RGB);
        Fl::use_high_res_GL(1);

        cln::default_float_format = cln::float_format(100);

        L = spdlog::stderr_color_mt("console");
    }

    Hub::~Hub() {
        if (!initialized) return;
        auto     end   = boost::chrono::process_real_cpu_clock::now();
        Duration d     = end - start;
        auto     end_u = boost::chrono::process_user_cpu_clock::now();
        Duration d_u   = end_u - start_u;
        auto     end_s = boost::chrono::process_system_cpu_clock::now();
        Duration d_s   = end_s - start_s;
        output("Time spent", "", fmt::format("{} real, {} user, {} system", d.count(), d_u.count(), d_s.count()), false);

        for (auto & o : outputs) {
            for (int i = o.label.size(); i < max_label_width; ++i) o.label.append(" ");
            L->info("{} : {}", o.label, o.value);
        }

        std::string diary;
        for (auto [k, v] : *this) diary += fmt::format(" {} {}", k, str(v));
        diary += " |";
        for (auto [k, ks, v, o] : outputs)
            if (o) diary += fmt::format(" {} {}", ks, str(v));
        auto f = spdlog::basic_logger_mt(fmt::format("{} {}", prog, version), "diary.log");
        f->set_pattern("%Y-%m-%d %H:%M:%S %n |%v");
        f->info(diary);
    }

    void Hub::init(std::string t, int argc, char ** argv, std::string c) {
        title   = std::move(t);
        help    = "Syntax : " + c;
        version = GIT_SHA1;

        auto                     argv_ = gsl::span<char *>(argv, argc);
        std::vector<std::string> fs;
        boost::split(fs, argv_[0], boost::is_any_of(R"(/\)"));
        prog        = fs.back();
        dir         = "output/" + prog + "/";
        mode_t mode = 0755u;
        mkdir("output", mode);
        mkdir(dir.c_str(), mode);
        mkdir((dir + "snapshots").c_str(), mode);

        std::vector<std::string> cs;
        if (!c.empty()) boost::split(cs, c, boost::is_any_of(", "), boost::token_compress_on);

        std::string getopt_arg("h");
        for (auto s : cs) {
            if (s.length() == 1) {
                getopt_arg += s.substr(0, 1);
                insert_or_assign(s[0], "0");
                has_arg[s[0]] = false;
            } else {
                getopt_arg += s.substr(0, 1) + ":";
                insert_or_assign(s[0], s.substr(2));
                has_arg[s[0]] = true;
            }
        }

        char ch;
        while ((ch = getopt(argc, argv, getopt_arg.c_str())) != -1) {
            if (ch == 'h') {
                L->info(help);
                exit(0);
            } else if (has_arg[ch]) {
                insert_or_assign(ch, optarg);
            } else {
                insert_or_assign(ch, "1");
            }
        }

        if (!empty()) {
            cs.clear();
            for (auto i : *this) cs.push_back(std::string(1, i.first) + "=" + i.second);
            title += " (" + boost::join(cs, ", ") + ")";
        }

        L = spdlog::stderr_color_mt(prog);

        cmd = argv_[0];
        for (int i = 1; i < argc; ++i) {
            cmd += " ";
            cmd += argv_[i];
        }
        output("Command line", "", cmd, false);
        output("Code version", "", version, false);
        output("Image title", "", title, false);

        start       = boost::chrono::process_real_cpu_clock::now();
        start_u     = boost::chrono::process_user_cpu_clock::now();
        start_s     = boost::chrono::process_system_cpu_clock::now();
        initialized = true;
    }

    void Hub::output_str(std::string l, std::string ls, std::string s, bool o) {
        if (l.size() > max_label_width) max_label_width = l.size();
        outputs.push_back({l, ls, s, o});
    }

    Hub H;
} // namespace vb
