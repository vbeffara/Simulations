#include <boost/algorithm/string.hpp>
#include <getopt.h>
#include <gsl/gsl>
#include <vb/util/CL_Parser.h>

namespace vb {
    void CL_Parser::init(std::string t, int argc, char **argv, std::string c) {
        help  = "Syntax : " + c;
        title = std::move(t);

        auto argv_ = gsl::span<char *>(argv, argc);

        std::vector<std::string> fs;
        boost::split(fs, argv_[0], boost::is_any_of(R"(/\)"));

        prog = fs.back();
        dir  = "output/" + prog + "/";

        std::vector<std::string> cs;
        if (!c.empty()) boost::split(cs, c, boost::is_any_of(", "), boost::token_compress_on);

        std::string getopt_arg("h");
        insert_or_assign('h', "0");

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
            if (has_arg[ch]) {
                insert_or_assign(ch, optarg);
            } else {
                insert_or_assign(ch, "1");
            }
        }

        if (!empty()) {
            cs.clear();
            for (const auto &[k, v] : *this) cs.push_back(std::string(1, k) + "=" + v);
            title += " (" + boost::join(cs, ", ") + ")";
        }

        cmd = argv_[0];
        for (int i = 1; i < argc; ++i) {
            cmd += " ";
            cmd += argv_[i];
        }
    }
} // namespace vb
