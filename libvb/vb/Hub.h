#pragma once /// \file

#include <vb/CL_Parser.h>
#include <vb/fmt.h>
#include <sstream>

namespace vb {
	class Hub : public CL_Parser {
	public:
		Hub (int argc, char ** argv, fmt t,         std::string c = "", std::string h = "") : Hub (argc,argv,str(t),c,h) {}

		Hub (int argc, char ** argv, std::string t, std::string c = "", std::string h = "") : CL_Parser (argc,argv,c,h), title(t) {
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