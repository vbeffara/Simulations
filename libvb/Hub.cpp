#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <boost/algorithm/string.hpp>
#include <spdlog/spdlog.h>
#include <sys/stat.h>
#include <vb/util/Hub.h>
#include <vb/util/mp.h>
#include <vector>

namespace vb {
    Hub::Hub(std::string t, int argc, char **argv, std::string c) : CL_Parser(std::move(t), argc, argv, std::move(c)) {
        if (at('h')) {
            spdlog::info(help);
            exit(0);
        };

        mode_t mode = 0755U;
        mkdir("output", mode);
        mkdir(dir.c_str(), mode);
        mkdir((dir + "snapshots").c_str(), mode);
        chdir(dir.c_str());

        output("Command line", "", cmd, false);
        output("Code version", "", version(), false);
        output("Image title", "", title, false);

        start   = boost::chrono::process_real_cpu_clock::now();
        start_u = boost::chrono::process_user_cpu_clock::now();
        start_s = boost::chrono::process_system_cpu_clock::now();
    }

    Hub::~Hub() {
        auto     end   = boost::chrono::process_real_cpu_clock::now();
        auto     end_u = boost::chrono::process_user_cpu_clock::now();
        auto     end_s = boost::chrono::process_system_cpu_clock::now();
        Duration d = end - start, d_u = end_u - start_u, d_s = end_s - start_s;
        output("Time spent", "", fmt::format("{} real, {} user, {} system", d.count(), d_u.count(), d_s.count()), false);

        auto format = fmt::format("{{:<{}}} : {{}}", max_label_width);
        for (const auto &[k, ks, v, o] : outputs) spdlog::info(fmt::format(format, k, v));

        chdir("../..");
    }
} // namespace vb
