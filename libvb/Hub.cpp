#include <vb/Hub.h>
#include <vector>
#include <getopt.h>
#include <boost/algorithm/string.hpp>

namespace vb {
	Hub::Hub (std::string t, int argc, char ** argv, std::string c) : title(t), help("Syntax : " + c) {
		std::vector<std::string> cs;
		boost::split (cs, c, boost::is_any_of(", "), boost::token_compress_on);

		std::string getopt_arg ("h");
		for (auto s : cs) {
			if (s.length() == 1)	{ getopt_arg += s.substr(0,1);      	(*this)[s[0]] = "0";        	has_arg[s[0]] = false; }
			else                	{ getopt_arg += s.substr(0,1) + ":";	(*this)[s[0]] = s.substr(2);	has_arg[s[0]] = true; }
		}

		int ch;
		while ((ch = getopt(argc,argv,getopt_arg.c_str())) != -1) {
			if (ch == 'h')       	{ std::cerr << help << std::endl; exit(0); } 
			else if (has_arg[ch])	{ (*this)[ch] = optarg; }
			else                 	{ (*this)[ch] = "1"; }
		}

		if (size()) {
			cs.clear();
			for (auto i : *this) cs.push_back (std::string(1,i.first) + "=" + i.second);
			title += " (" + boost::join (cs,", ") + ")";
		}
	}
}
