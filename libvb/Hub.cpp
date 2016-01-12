#include <boost/algorithm/string.hpp>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <sys/stat.h>
#include <vb/Hub.h>
#include <iostream>
#include <vector>
#include <getopt.h>

namespace vb {
	Hub::Hub () {}

	void Hub::init (std::string t, int argc, char ** argv, std::string c) {
		title = t; help = "Syntax : " + c;

		std::vector<std::string> fs;	boost::split (fs, argv[0], boost::is_any_of("/\\"));
		prog = fs.back(); dir = "output/" + prog + "/";
		mode_t mode = S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH;
		mkdir("output", mode); mkdir(dir.c_str(), mode); mkdir((dir + "snapshots").c_str(), mode);

		std::vector<std::string> cs;	if (c.size()) boost::split (cs, c, boost::is_any_of(", "), boost::token_compress_on);

		std::string getopt_arg ("h");
		for (auto s : cs) {
			if (s.length() == 1)	{ getopt_arg += s.substr(0,1);      	(*this)[s[0]] = "0";        	has_arg[s[0]] = false; }
			else                	{ getopt_arg += s.substr(0,1) + ":";	(*this)[s[0]] = s.substr(2);	has_arg[s[0]] = true; }
		}

		char ch;
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

	    Fl::gl_visual (FL_RGB);
	    Fl::use_high_res_GL (1);
	}

	Hub H;
}
