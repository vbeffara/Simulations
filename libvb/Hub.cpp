#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <boost/algorithm/string.hpp>
#include <sqlite_modern_cpp.h>
#include <sys/stat.h>
#include <vb/Hub.h>
#include <vb/mp.h>
#include <vector>

namespace vb {
    Hub::~Hub() {
        if (!initialized) return;
        auto     end   = boost::chrono::process_real_cpu_clock::now();
        auto     end_u = boost::chrono::process_user_cpu_clock::now();
        auto     end_s = boost::chrono::process_system_cpu_clock::now();
        Duration d = end - start, d_u = end_u - start_u, d_s = end_s - start_s;
        output("Time spent", "", fmt::format("{} real, {} user, {} system", d.count(), d_u.count(), d_s.count()), false);

        auto        format = fmt::format("{{:<{}}} : {{}}", max_label_width);
        std::string os, ls;
        for (const auto &[k, ks, v, o] : outputs) {
            L->info(fmt::format(format, k, v));
            if (o) {
                os += ",?";
                ls += "," + ks;
            }
        }

        sqlite::database db("diary.db");

        db << "create table if not exists cmds ("
              "  cmd_id integer primary key autoincrement not null,"
              "  prog, version, args);";
        db << "create table if not exists runs ("
              "  id integer primary key autoincrement not null,"
              "  date timestamp default (datetime('now')),"
              "  cmd_id, a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z);";

        std::vector<std::string> as;
        for (const auto &[k, v] : *this) as.push_back(fmt::format("{}={}", k, std::string(v)));
        std::string args = boost::join(as, ", ");

        std::optional<int64_t> id;
        db << "select cmd_id from cmds where prog = ? and version = ? and args = ?;" << prog << version << args >>
            [&](int64_t i) { id = i; };

        if (!id) {
            db << "insert into cmds (prog,version,args) values (?,?,?);" << prog << version << args;
            id = db.last_insert_rowid();
        }

        auto tmp = db << "insert into runs (cmd_id" + ls + ") values (?" + os + ");" << *id;
        for (const auto &[k, ks, v, o] : outputs)
            if (o) tmp << v;
    }

    void Hub::init(std::string t, int argc, char **argv, std::string c) {
        CL_Parser::init(t, argc, argv, c);
        L = spdlog::stderr_color_mt(prog);

        if (at('h')) {
            L->info(help);
            exit(0);
        };

        Fl::gl_visual(FL_RGB);
        Fl::use_high_res_GL(1);

        real_t::default_precision(100);
        complex_t::default_precision(100);

        version = GIT_SHA1;

        mode_t mode = 0755u;
        mkdir("output", mode);
        mkdir(dir.c_str(), mode);
        mkdir((dir + "snapshots").c_str(), mode);

        output("Command line", "", cmd, false);
        output("Code version", "", version, false);
        output("Image title", "", title, false);

        start       = boost::chrono::process_real_cpu_clock::now();
        start_u     = boost::chrono::process_user_cpu_clock::now();
        start_s     = boost::chrono::process_system_cpu_clock::now();
        initialized = true;
    }

    void Hub::output_str(const std::string &l, const std::string &ls, const std::string &s, bool out) {
        if (l.size() > max_label_width) max_label_width = l.size();
        outputs.push_back({l, ls, s, out});
    }

    Hub H;
} // namespace vb
