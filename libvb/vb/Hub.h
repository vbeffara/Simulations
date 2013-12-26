#pragma once /// \file

#include <vb/CL_Parser.h>
#include <vb/fmt.h>
#include <sstream>

namespace vb {
	class Hub : public CL_Parser {
	public:
		Hub (boost::format t, int argc, char ** argv, std::string c = "") : Hub (str(t),argc,argv,c) {}

		Hub (std::string t,   int argc, char ** argv, std::string c = "") : CL_Parser (argc,argv,c), title(t) {
			std::ostringstream o;
			if (params.size() + has_arg.size()) {
				bool first = true;
				o << " (";
				for (auto i : params) {
					o << (first ? "" : ",") << i.first << "=" << i.second;
					first = false;
				}
				o << ")";
			}
			title += o.str();
		}

		std::string title;
	};
}