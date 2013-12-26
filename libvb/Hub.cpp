#include <vb/Hub.h>
#include <iostream>
#include <sstream>
#include <getopt.h>

namespace vb {
	Hub::Hub (std::string t, int argc, char ** argv, std::string c) : title(t), help("Syntax : " + c), getopt_arg("h") {
			size_t cut;
			while ((cut=c.find(",")) != std::string::npos) { newparam (c.substr(0,cut)); c = c.substr(cut+1); }
			if (c.length()) newparam (c);

			parse(argc,argv);

			std::ostringstream o;
			if (size()) {
				bool first = true;
				o << " (";
				for (auto i : *this) { o << (first ? "" : ",") << i.first << "=" << i.second; first = false; }
				o << ")";
			}
			title += o.str();
		}

	void Hub::newparam (const std::string &s) {
		if (s.length() == 1)	{ getopt_arg += s.substr(0,1);      	(*this)[s[0]] = "0";        	has_arg[s[0]] = false; }
		else                	{ getopt_arg += s.substr(0,1) + ':';	(*this)[s[0]] = s.substr(2);	has_arg[s[0]] = true; }
	}

	void Hub::parse (int argc, char **argv) {
		int ch;
		while ((ch = getopt(argc,argv,getopt_arg.c_str())) != -1) {
			if (ch == 'h')       	{ std::cerr << help << std::endl; exit(0); } 
			else if (has_arg[ch])	{ (*this)[ch] = optarg; }
			else                 	{ (*this)[ch] = "1"; }
		}
	}
}
