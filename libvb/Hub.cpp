#include <boost/algorithm/string.hpp>
#include <cln/cln.h>
#include <FL/Fl.H>
#include <FL/Fl_Gl_Window.H>
#include <gsl/gsl>
#include <sys/stat.h>
#include <vb/Hub.h>
#include <iostream>
#include <vector>
#include <getopt.h>

#define STR(s) #s
#define XTR(s) STR(s)
#define GIT_SHA1_lit XTR(GIT_SHA1)

namespace vb {
	Hub::Hub () {
	    Fl::gl_visual (FL_RGB);
	    #ifndef NO_RETINA
	    Fl::use_high_res_GL (1);
	    #endif

	    cln::default_float_format = cln::float_format(100);

	    L = spdlog::stdout_logger_mt ("console", true);
	}

	Hub::~Hub () {
		if (!initialized) return;
		auto end = boost::chrono::process_real_cpu_clock::now(); Duration d = end - start;
		auto end_u = boost::chrono::process_user_cpu_clock::now(); Duration d_u = end_u - start_u;
		auto end_s = boost::chrono::process_system_cpu_clock::now(); Duration d_s = end_s - start_s;
		L->info ("Time spent   : {} real, {} user, {} system", d.count(), d_u.count(), d_s.count());
	}

	void Hub::init (std::string t, int argc, char ** argv, std::string c) {
		title = std::move(t); help = "Syntax : " + c; version = GIT_SHA1_lit;

		auto argv_ = gsl::span <char*> (argv,argc);
		std::vector<std::string> fs;	boost::split (fs, argv_[0], boost::is_any_of(R"(/\)"));
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
			if (ch == 'h')       	{ L->info (help); exit(0); }
			else if (has_arg[ch])	{ (*this)[ch] = optarg; }
			else                 	{ (*this)[ch] = "1"; }
		}

		if (size()) {
			cs.clear();
			for (auto i : *this) cs.push_back (std::string(1,i.first) + "=" + i.second);
			title += " (" + boost::join (cs,", ") + ")";
		}

	    L = spdlog::stdout_logger_mt (prog, true);

		cmd = argv_[0]; for (int i=1; i<argc; ++i) { cmd += " "; cmd += argv_[i]; }
		L->info ("Command line : {}", cmd);
		L->info ("Code version : {}", version);
		L->info ("Image title  : {}", title);

		start = boost::chrono::process_real_cpu_clock::now();
		start_u = boost::chrono::process_user_cpu_clock::now();
		start_s = boost::chrono::process_system_cpu_clock::now();
		initialized = true;
	}

	Hub H;
}
