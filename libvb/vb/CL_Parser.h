#pragma once /// \file

#include <vb/Hub.h>

namespace vb {
	class CL_Parser : public Hub {
	public:
		CL_Parser (int argc, char** argv, std::string syntax) : Hub ("(no title set)", argc, argv, syntax) {};
		Value operator() (char c) { return at(c); }
	};
}
