#include <boost/algorithm/string.hpp>
#include <getopt.h>
#include <gsl/gsl>
#include <vb/util/CL_Parser.h>

namespace vb {
    CL_Parser::CL_Parser(std::string t, int argc, char **argv, std::string c) {
        std::vector<std::string> args;
        for (const auto &a : gsl::span(argv, argc)) args.push_back(a);

        help  = "Syntax : " + c;
        title = std::move(t);

        std::vector<std::string> fs;
        boost::split(fs, args[0], boost::is_any_of(R"(/\)"));

        prog = fs.back();
        dir  = "output/" + prog + "/";

        std::vector<std::string> cs;
        if (!c.empty()) boost::split(cs, c, boost::is_any_of(", "), boost::token_compress_on);

        std::string getopt_arg("h");
        insert_or_assign('h', "0");

        std::map<char, bool> has_arg;
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
        optind = 1;
        while ((ch = char(getopt(argc, argv, getopt_arg.c_str()))) != -1) insert_or_assign(ch, has_arg[ch] ? optarg : "1");

        if (!empty()) {
            cs.clear();
            for (const auto &[k, v] : *this) cs.push_back(std::string(1, k) + "=" + v);
            title += " (" + boost::join(cs, ", ") + ")";
        }

        // TODO use fmt join
        std::string sep;
        for (const auto &a : args) {
            cmd += sep + a;
            sep = " ";
        }
    }
} // namespace vb
